/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50528
Source Host           : localhost:3306
Source Database       : analysis

Target Server Type    : MYSQL
Target Server Version : 50528
File Encoding         : 65001

Date: 2015-11-01 10:51:38
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for t_trade_detail
-- ----------------------------
DROP TABLE IF EXISTS `t_trade_detail`;
CREATE TABLE `t_trade_detail` (
  `yyyymmdd` int(11) NOT NULL,
  `platform` varchar(32) DEFAULT NULL,
  `zone_id` varchar(16) NOT NULL,
  `time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `s_uid` varchar(64) NOT NULL,
  `t_uid` varchar(64) NOT NULL,
  `s_rid` varchar(16) NOT NULL,
  `t_rid` varchar(16) NOT NULL,
  `s_ip` varchar(32) NOT NULL,
  `t_ip` varchar(32) NOT NULL,
  `i_list` varchar(255) NOT NULL,
  `o_list` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for t_trade_history
-- ----------------------------
DROP TABLE IF EXISTS `t_trade_history`;
CREATE TABLE `t_trade_history` (
  `platform` varchar(32) DEFAULT NULL,
  `zone_id` varchar(16) NOT NULL,
  `s_uid` varchar(64) NOT NULL,
  `t_uid` varchar(64) NOT NULL,
  `s_rid` varchar(16) NOT NULL,
  `t_rid` varchar(16) NOT NULL,
  `s_ip` varchar(32) NOT NULL,
  `t_ip` varchar(32) NOT NULL,
  `s_viplevel` int(11) NOT NULL,
  `t_viplevel` int(11) NOT NULL,
  `i_list` varchar(255) NOT NULL,
  `o_list` varchar(255) NOT NULL,
  `map` int(11) NOT NULL,
  `friend_degree` int(11) NOT NULL,
  `time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for t_trade_master
-- ----------------------------
DROP TABLE IF EXISTS `t_trade_master`;
CREATE TABLE `t_trade_master` (
  `yyyymmdd` int(11) NOT NULL,
  `platform` varchar(64) DEFAULT NULL,
  `uid` varchar(64) NOT NULL,
  `trade_count` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
