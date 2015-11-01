/*
Navicat MySQL Data Transfer

Source Server         : 192.168.3.59
Source Server Version : 50528
Source Host           : 192.168.3.59:3306
Source Database       : jg_chat

Target Server Type    : MYSQL
Target Server Version : 50528
File Encoding         : 65001

Date: 2014-02-20 09:09:22
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `tb_account`
-- ----------------------------
DROP TABLE IF EXISTS `tb_account`;
CREATE TABLE `tb_account` (
  `Passport` bigint(20) NOT NULL AUTO_INCREMENT,
  `AcctName` varchar(32) NOT NULL,
  `Password` char(32) NOT NULL,
  `Nick` varchar(64) DEFAULT NULL,
  `Signature` varchar(256) DEFAULT NULL,
  `CreateTime` timestamp NULL DEFAULT NULL,
  `LastLoginTime` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`Passport`),
  KEY `idx_accountName` (`AcctName`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tb_account
-- ----------------------------

-- ----------------------------
-- Table structure for `tb_friend`
-- ----------------------------
DROP TABLE IF EXISTS `tb_friend`;
CREATE TABLE `tb_friend` (
  `Passport` bigint(20) NOT NULL DEFAULT '0',
  `FriendID` bigint(20) NOT NULL DEFAULT '0',
  `CreateTime` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Passport`,`FriendID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tb_friend
-- ----------------------------

-- ----------------------------
-- Table structure for `tb_group`
-- ----------------------------
DROP TABLE IF EXISTS `tb_group`;
CREATE TABLE `tb_group` (
  `GroupID` bigint(20) NOT NULL AUTO_INCREMENT,
  `Nick` varchar(64) DEFAULT NULL,
  `Signature` varchar(256) DEFAULT NULL,
  `CreateTime` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`GroupID`),
  KEY `idx_nick` (`Nick`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tb_group
-- ----------------------------

-- ----------------------------
-- Table structure for `tb_groupdetail`
-- ----------------------------
DROP TABLE IF EXISTS `tb_groupdetail`;
CREATE TABLE `tb_groupdetail` (
  `GroupID` bigint(20) NOT NULL DEFAULT '0',
  `Passport` bigint(20) NOT NULL DEFAULT '0',
  `CreateTime` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`GroupID`,`Passport`),
  KEY `idx_passport` (`Passport`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tb_groupdetail
-- ----------------------------

-- ----------------------------
-- Table structure for `tb_subaccount`
-- ----------------------------
DROP TABLE IF EXISTS `tb_subaccount`;
CREATE TABLE `tb_subaccount` (
  `Passport` bigint(20) NOT NULL DEFAULT '0',
  `SubAcctName` varchar(32) NOT NULL,
  `SubAcctType` tinyint(4) NOT NULL,
  `LastLoginTime` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`Passport`),
  KEY `idx_subAcctName_type` (`SubAcctName`,`SubAcctType`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tb_subaccount
-- ----------------------------

-- ----------------------------
-- Procedure structure for `proc_AuthenticateAccount`
-- ----------------------------
DROP PROCEDURE IF EXISTS `proc_AuthenticateAccount`;
DELIMITER ;;
CREATE DEFINER=`GameAdmin`@`%` PROCEDURE `proc_AuthenticateAccount`(IN inAcctName varchar(32),IN inPwdMd5 char(32))
proc:
BEGIN
	declare err_account_notfound int default 1;
	declare err_password_mismatch int default 2;
	
	declare passportID bigint;
	declare pwdMd5 char(32) default NULL;
	declare nickName varchar(64);
	declare mySignature varchar(256);

	select Passport,Password,Nick,Signature into passportID,pwdMd5,nickName,mySignature from tb_account where AcctName=inAcctName;

	if passportID is NULL THEN
		select func_CreateAccount(inAcctName,inPwdMd5,'no-nickName','no-mySignature',1) into passportID;
		if passportID < 0 then
			select -passportID,NULL,NULL,NULL;
		else
			select 0,passportID,'no-nickName','no-mySignature';
		end if;
		leave proc;
	end if;
	

	if not pwdMd5 = inPwdMd5 then
		select err_password_mismatch,NULL,NULL,NULL;
		leave proc;
	end if;

	update tb_account set LastLoginTime=NOW() where Passport=passportID;
	select 0,passportID,nickName,mySignature;
	leave proc;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `proc_AuthenticateSubAccount`
-- ----------------------------
DROP PROCEDURE IF EXISTS `proc_AuthenticateSubAccount`;
DELIMITER ;;
CREATE DEFINER=`GameAdmin`@`%` PROCEDURE `proc_AuthenticateSubAccount`(IN `@subAcctName` varchar(32), IN `@subAcctType` int)
proc:
BEGIN
	declare err_account_notfound int default 1;

	declare passportID bigint default NULL;

	select Passport into passportID from tb_subaccount where SubAcctName=@subAcctName and SubAcctType=@subAcctType;
	if passportID is NULL then
		select err_account_notfound,NULL,NULL,NULL;
		leave proc;
	end if;

	update tb_subaccount set LastLoginTime=NOW() where Passport=passportID;
	update tb_account set LastLoginTime=NOW() where Passport=passportID;
	
	select result=0,Passport=passportID,Nick,Signature from tb_account where Passport=passportID;
	leave proc;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `proc_SetAccountInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `proc_SetAccountInfo`;
DELIMITER ;;
CREATE DEFINER=`GameAdmin`@`%` PROCEDURE `proc_SetAccountInfo`(IN `inPassportID` bigint,IN `inNick` varchar(64),IN `inSignature` varchar(256))
proc:
BEGIN
	declare err_account_notfound int default 1;
	if not EXISTS (select count(*) from tb_account where Passport=inPassportID) then
		select err_account_notfound;
		leave proc;
	end if;
	
	update tb_account set Nick=inNick,Signature=inSignature where Passport=inPassportID;
	select 0;
	leave proc;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for `func_CreateAccount`
-- ----------------------------
DROP FUNCTION IF EXISTS `func_CreateAccount`;
DELIMITER ;;
CREATE DEFINER=`GameAdmin`@`%` FUNCTION `func_CreateAccount`(inAcctName varchar(32),inPwdMd5 char(32),inNick varchar(64),inSignature varchar(256),inForce int) RETURNS bigint(20)
BEGIN
	declare err_account_already_exists int default 100;

	declare rowCount int;

	if inForce = 0 then
		select count(*) into rowCount from tb_account where AcctName = inAcctName;
		if rowCount > 0 then
			return -err_account_already_exists;
		end if;
	end if;

	insert into tb_account(AcctName,Password,Nick,Signature,CreateTime,LastLoginTime)
	values(inAcctName,inPwdMd5,inNick,inSignature,NOW(),NOW());
	
	return LAST_INSERT_ID();
END
;;
DELIMITER ;
