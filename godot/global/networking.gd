extends Node

signal connected
signal disconnected

const Protocol = preload('res://native/protocol.gdns')

const host = 'adrestia.neynt.ca'
const port = 16969
const handler_key = 'api_handler_name'

# jim: So the keepalive works as follows.
# - We keep track of the when we've last sent and received data.
# - If we don't receive data for [timeout_ms], we're disconnected.
# - If we haven't sent data for [floop_interval_ms], we send a floop to ensure
# that we're still connected.
# - If we get disconnected, we retry on [retry_sec] intervals.

const timeout_ms = 10000
const floop_interval_ms = 2000
const retry_sec = 5.0

onready var g = get_node('/root/global')

var peer
var data_buffer
var protocol
var handlers = {}

var connect_timer
var last_recv_ms = 0
var last_send_ms = 0

const OFFLINE = 0
const CONNECTING = 1
const ONLINE = 2

var status = OFFLINE

func _ready():
	connect_timer = Timer.new()
	connect_timer.set_one_shot(true)
	connect_timer.set_timer_process_mode(0)
	connect_timer.set_wait_time(retry_sec)
	connect_timer.connect('timeout', self, 'reconnect')
	add_child(connect_timer)
	self.protocol = Protocol.new()
	self.peer = StreamPeerTCP.new()
	reconnect()

func _process(time):
	if peer.get_status() == StreamPeerTCP.STATUS_CONNECTING: return
	if (peer.get_status() != StreamPeerTCP.STATUS_CONNECTED
			|| OS.get_ticks_msec() - last_recv_ms > timeout_ms):
		status = OFFLINE
		emit_signal('disconnected')
		print('Disconnected. Will retry in %.1f seconds.' % [retry_sec])
		connect_timer.start()
		set_process(false)
		return
	
	if status == OFFLINE:
		status = CONNECTING
		print('Connecting...')
		establish_connection(funcref(self, 'on_network_ready'))

	if OS.get_ticks_msec() - last_send_ms > 2000:
		floop(funcref(self, 'on_floop'))

	var bytes = self.peer.get_available_bytes()
	if bytes > 0:
		last_recv_ms = OS.get_ticks_msec()
		match self.peer.get_partial_data(bytes):
			[var err, var data]:
				self.data_buffer.append_array(data)

		# TODO: jim: n^2 work if we have many small chunks of data. Hopefully that
		# doesn't happen.
		var i = 0
		while i < self.data_buffer.size():
			if self.data_buffer[i] == 10: # newline
				var message = self.data_buffer.subarray(0, i - 1).get_string_from_utf8()
				var json = JSON.parse(message).result
				var handler = json[handler_key]
				if handler in handlers:
					if handlers[handler].call_func(json):
						handlers.erase(handler)
				break
			i += 1

		if i + 1 >= self.data_buffer.size():
			# Edge case for when we consume ALL of the available data
			# (this actually happens most of the time)
			self.data_buffer.resize(0)
		else:
			self.data_buffer = self.data_buffer.subarray(i + 1, -1)

func to_packet(s):
	return (s + '\n').to_utf8()

func handler_name(request):
	return JSON.parse(request).result[handler_key]

func reconnect():
	print('Attempting to reconnect.')
	self.peer.connect_to_host(host, port)
	self.data_buffer = PoolByteArray()
	last_send_ms = OS.get_ticks_msec()
	last_recv_ms = OS.get_ticks_msec()
	set_process(true)

func on_network_ready(response):
	if g.auth_uuid != null:
		authenticate(g.auth_uuid, g.auth_pwd, funcref(self, 'on_authenticated'))
	else:
		gen_auth_pwd()
		register_new_account(g.auth_pwd, funcref(self, 'on_account_created'))

func gen_auth_pwd():
	g.auth_pwd = ''
	for _i in range(24):
		g.auth_pwd += char(randi() % 26 + 0x61) # Random lowercase letter

func on_account_created(response):
	g.auth_uuid = response.uuid
	g.user_name = response.user_name
	g.tag = response.tag
	after_auth()

func on_authenticated(response):
	if response.api_code == 401:
		# TODO: jim: this should not happen unless we clear the database. warn user
		# that their account has been nuked in that case?
		gen_auth_pwd()
		register_new_account(g.auth_pwd, funcref(self, 'on_account_created'))
		return
	g.user_name = response.user_name
	g.tag = response.tag
	after_auth()

func after_auth():
	g.save()
	status = ONLINE
	print('Connected!')
	emit_signal('connected')

func on_floop(response):
	pass

func register_handlers(obj, on_connected, on_disconnected):
	self.connect('connected', obj, on_connected)
	self.connect('disconnected', obj, on_disconnected)
	if status == ONLINE:
		obj.call(on_connected)
	else:
		obj.call(on_disconnected)

# Actual API starts here.
# If a callback returns true, it will only be used to handle a single response.
# Otherwise it will stick around.

func api_call_base(name, args, callback):
	last_send_ms = OS.get_ticks_msec()
	if status == OFFLINE:
		print('Network call %s failed because disconnected.' % [name])
		return false
	var request = protocol.callv('create_%s_call' % [name], args)
	var handler = handler_name(request)
	handlers[handler] = callback
	if self.peer.get_status() != StreamPeerTCP.STATUS_CONNECTED:
		return false
	self.peer.put_data(to_packet(request))
	return true

func floop(callback):
	return api_call_base('floop', [], callback)

func establish_connection(callback):
	return api_call_base('establish_connection', [], callback)

func register_new_account(password, callback):
	return api_call_base('register_new_account', [password], callback)

func authenticate(uuid, password, callback):
	return api_call_base('authenticate', [uuid, password], callback)

func change_user_name(user_name, callback):
	return api_call_base('change_user_name', [user_name], callback)

func matchmake_me(books, callback):
	return api_call_base('matchmake_me', [books], callback)
