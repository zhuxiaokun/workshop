
--LOG_FATAL = 1
--LOG_ERROR = 2
--LOG_WARN  = 4
--LOG_INFO  = 8
--LOG_DEBUG = 16
--LOG_CONSOLE = (1<<31)
--LOG_ALL   = (~0)

local host = '127.0.0.1'
local port = 3306
local dbname = 'analysis'
local user = 'root'
local passwd = '123456'
local cnnLifeTime = 30*60

local TIMER_RESET_CNN = 1

function insertTradeHistory(cnn, t)
	local zone_id = t.zone_id
	local s_uid = t.s_uid
	local t_uid = t.t_uid
	local s_rid = t.s_rid
	local t_rid = t.t_rid
	local s_ip = t.s_ip
	local t_ip = t.t_ip
	local s_viplevel = t.s_viplevel
	local t_viplevel = t.t_viplevel
	local i_list = t.i_list
	local o_list = t.o_list
	local map = t.map
	local friend_degree = t.friend_degree
	local time = t.time
	
	local sql = string.format('insert into t_trade_history(zone_id,s_uid,t_uid,s_rid,t_rid,s_ip,t_ip,s_viplevel,t_viplevel,i_list,o_list,map,friend_degree,time) \
		values("%s","%s","%s","%s","%s","%s","%s",%d,%d,"%s","%s",%d,%d,"%s")',
		zone_id,s_uid,t_uid,s_rid,t_rid,s_ip,t_ip,s_viplevel,t_viplevel,i_list,o_list,map,friend_degree,time)
	
	local ret = cnn:execute(sql)
	if not ret then
		Log(LOG_CONSOLE+LOG_ERROR, 'error: insert in t_trade_history, %s, %s', s_uid, cnn:getLastError())
	end
	return ret
end

application_configure = {
	log_level = LOG_FATAL+LOG_ERROR+LOG_WARN,
	status_file = '..\\.log_processer_status.dat',
	
	onInitialize = function(self)
		self.lineCount = 0
		self.cnn = mysql()
		if not self.cnn:connect(host, port, dbname, user, passwd) then
			Log(LOG_ERROR, 'error: connect to database %s:%d %s, %s', host, port, dbname, self.cnn:getLastError())
		end
		self.cnnReady = self.cnn:isReady()
		local duration = cnnLifeTime
		if not self.cnnReady then
			duration = 20.0
		end
		self.this:startTimer(TIMER_RESET_CNN, cookie, duration, null)
		return true
	end,
	
	onFinalize = function(self)
		self.cnn:close()
		return true
	end,
	
	process = function(self, lineMap)
		local verb = lineMap:get('verb')
		if verb ~= 'trade' then
			return true
		end
		
		self.lineCount = self.lineCount + 1
		if self.lineCount % 100 == 0 then
			Log(LOG_ERROR, 'debug: line count %d', self.lineCount)
		end
		
		local ti = {
			zone_id = lineMap:get('server'),
			s_uid = lineMap:get('source'),
			t_uid = lineMap:get('target'),
			s_rid = lineMap:get('srcPlayer'),
			t_rid = lineMap:get('dstPlayer'),
			s_ip = lineMap:get('sip'),
			t_ip = lineMap:get('tip'),
			s_viplevel = tonumber(lineMap:get('sviplevel')),
			t_viplevel = tonumber(lineMap:get('tviplevel')),
			i_list = lineMap:get('in'),
			o_list = lineMap:get('out'),
			map = tonumber(lineMap:get('map')),
			friend_degree = tonumber(lineMap:get('friendDegree')),
			time = time2str(tonumber(lineMap:get('time'))),
		}
		if not ti.zone_id then
			Log(LOG_ERROR, 'error: %s', lineMap:toString())
			return true
		end
		
		insertTradeHistory(self.cnn, ti)
		return true
	end,
	
	onTimer = {
		[TIMER_RESET_CNN] = function(self,tid,cookie,data)
			Log(LOG_ERROR, 'debug: reconnecting to database...')
			if not self.cnn:reconnect() then
				Log(LOG_ERROR, 'error: connect to database %s:%d %s, %s', host, port, dbname, self.cnn:getLastError())
			end
			self.cnnReady = self.cnn:isReady()
			local duration = cnnLifeTime
			if not self.cnnReady then
				duration = 20.0
			end
			self.this:startTimer(TIMER_RESET_CNN, cookie, duration, null)
			return true
		end,
	},
	
	watch_folders = {
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server10\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1000\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1001\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1002\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1003\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1004\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1006\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1007\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1008\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1009\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server101\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1010\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1011\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1012\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1013\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1014\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1015\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1016\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1017\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server1018\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server11\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server112\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server125\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server132\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server139\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server145\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server153\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server159\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server169\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server179\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server189\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server2\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server20\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server201\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server215\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server228\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server243\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server256\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server272\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server292\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server3\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server317\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server340\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server363\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server386\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server39\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server4\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server405\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server424\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server442\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server455\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server471\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server48\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server486\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server5\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server501\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server514\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server520\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server526\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server543\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server56\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server560\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server567\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server579\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server588\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server593\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server6\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server604\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server611\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server619\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server627\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server638\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server643\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server652\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server660\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server666\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server675\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server683\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server691\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server699\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server7\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server71\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server713\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server723\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server730\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server735\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server739\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server745\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server749\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server753\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server757\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server761\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server765\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server770\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server775\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server779\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server784\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server788\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server792\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server797\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server8\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server801\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server805\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server809\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server813\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server818\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server823\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server828\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server834\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server840\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server846\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server852\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server857\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server862\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server866\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server870\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server875\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server879\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server88\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server886\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server888\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server891\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server895\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server899\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server903\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server907\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server912\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server917\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server920\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server923\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server926\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server929\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server933\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server935\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server937\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server940\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server942\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server946\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server948\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server950\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server952\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server954\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server956\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server958\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server960\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server962\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server964\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server965\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server966\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server967\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server968\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server969\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server970\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server971\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server972\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server973\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server974\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server975\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server976\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server977\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server978\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server979\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server980\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server981\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server982\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server983\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server984\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server985\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server986\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server987\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server988\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server989\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server990\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server991\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server992\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server993\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server994\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server995\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server996\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server997\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server998\\log', file='Analysis.log', from=20151020 },
		{ dir='D:\\tmp\\Data\\Analysis20151028\\Server999\\log', file='Analysis.log', from=20151020 },
	},
}
