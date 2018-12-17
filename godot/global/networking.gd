extends Node

const Protocol = preload('res://native/protocol.gdns')

var peer
var data_buffer
var protocol

func _process(time):
	var bytes = self.peer.get_available_bytes()
	if bytes > 0:
		print('Got %d bytes from server' % [bytes])

		match self.peer.get_partial_data(bytes):
			[var err, var data]:
				print(data)
				self.data_buffer.append_array(data)

		var i = 0
		while i < self.data_buffer.size():
			if self.data_buffer[i] == 10: # newline
				var message = self.data_buffer.subarray(0, i - 1).get_string_from_utf8()
				print(message)
				break
			i += 1

		if i + 1 >= self.data_buffer.size():
			# Edge case for when we consume ALL of the available data
			# (which is actually the case most of the time)
			self.data_buffer.resize(0)
		else:
			self.data_buffer = self.data_buffer.subarray(i + 1, -1)

func _ready():
	print('ready')
	set_process(true)
	self.protocol = Protocol.new()
	self.peer = StreamPeerTCP.new()
	self.peer.connect_to_host('127.0.0.1', 16969)
	self.data_buffer = PoolByteArray()

func to_packet(s):
	return (s + '\n').to_utf8()

func floop():
	if self.peer.get_status() != StreamPeerTCP.STATUS_CONNECTED:
		print(self.peer.get_status())
		return false

	self.peer.put_data(to_packet(protocol.make_floop_request()))
	return true
