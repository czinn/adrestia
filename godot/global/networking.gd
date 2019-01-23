extends Node

const Protocol = preload('res://native/protocol.gdns')

const host = '127.0.0.1'
const port = 16969
const handler_key = 'api_handler_name'

var peer
var data_buffer
var protocol
var handlers = {}

func _process(time):
	var bytes = self.peer.get_available_bytes()
	if bytes > 0:
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

func _ready():
	set_process(true)
	self.protocol = Protocol.new()
	self.peer = StreamPeerTCP.new()
	self.peer.connect_to_host(host, port)
	self.data_buffer = PoolByteArray()

func to_packet(s):
	return (s + '\n').to_utf8()

func handler_name(request):
	return JSON.parse(request).result[handler_key]

# Actual API starts here.
# If a callback returns true, it will only be used to handle a single response.
# Otherwise it will stick around.

func api_call_base(name, args, callback):
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
