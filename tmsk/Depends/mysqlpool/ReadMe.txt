
mysql的连接池

根据配置文件分配与管理连接
可以实现mysql数据库的分布式处理以及读写分离

配置文件以提供一个模板 mysqlpool.conf.lua
策略函数的参数是一个自定义的lua table，具体内容由应用程序提供
