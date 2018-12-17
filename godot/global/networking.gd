extends Object

const Protocol = preload('res://native/protocol.gdns')

var peer
var protocol

func to_packet(s):
	return (s + '\n').to_utf8()

func floop():
	if self.peer.get_status() != StreamPeerTCP.STATUS_CONNECTED:
		print(self.peer.get_status())
		return false

	self.peer.put_data(to_packet(protocol.make_floop_request()))
	return true

func _init():
	print('ready')
	self.protocol = Protocol.new()
	self.peer = StreamPeerTCP.new()
	self.peer.connect_to_host('127.0.0.1', 16969)
