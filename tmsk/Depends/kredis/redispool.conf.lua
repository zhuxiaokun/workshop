
lifetime = 30*60

connections = {
	['local'] = {
		server = '127.0.0.1',
		port = 6379,
		max = 10,
	},
	--['192.168.4.232'] = {
	--	server = '127.0.0.1',
	--	port = 6379,
	--	max = 10,
	--},
}

--default = 'local'

policy = {
	master = function(self, request)
		return 'local'
	end,
	slave = function(self, request)
		return 'local'
	end,
}
