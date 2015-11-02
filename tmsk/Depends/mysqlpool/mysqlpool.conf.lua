
lifetime = 30*60

default = {
	server = '192.168.8.116',
	port = 3306,
	dbname = 'jg_im_20141030',
	charset = 'utf8',
	user = 'GameAdmin',
	password = 'jiguang',
}

connections = {
	['192.168.8.116'] = {
		server = '192.168.8.116',
		port = 3306,
		dbname = 'jg_im_20141030',
		charset = 'utf8',
		user = 'GameAdmin',
		password = 'jiguang',
		max = 10,
	},
	['192.168.4.232'] = {
		server = '192.168.4.232',
		port = 3306,
		dbname = 'sj_gamedb',
		charset = 'utf8',
		user = 'GameAdmin',
		password = 'jiguang',
		max = 10,
	},
}

policy = {
	-- request is a table
	-- now it has two fields: value, readOnly
	-- and maybe has more fields if needed
	master = function(self, request)
		return '192.168.8.116'
	end,
	group = function(self, request)
		local index = hash_i64(request.value, 2)
		if index == 0 then
			return '192.168.8.116'
		else
			return '192.168.4.232'
		end
	end,
	account = function(self, request)
		local index = hash_i64(request.value, 2)
		if index == 0 then
			return '192.168.8.116'
		else
			return '192.168.4.232'
		end
	end,
	uniqueCode = function(self, request)
		local index = hash_string(request.value, 2)
		if index == 0 then
			return '192.168.8.116'
		else
			return '192.168.8.232'
		end
	end,
}
