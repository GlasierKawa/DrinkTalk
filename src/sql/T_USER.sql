/*
Navicat MySQL Data Transfer

Source Server         : 111
Source Server Version : 50568
Source Host           : 121.36.69.144:3306
Source Database       : tengxun

Target Server Type    : MYSQL
Target Server Version : 50568
File Encoding         : 65001

Date: 2021-03-25 19:20:05
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for T_USER
-- ----------------------------
DROP TABLE IF EXISTS `T_USER`;
CREATE TABLE `T_USER` (
  `f_user_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID\n',
  `f_password` varchar(64) NOT NULL COMMENT '用户密码',
  `f_user_name` varchar(64) NOT NULL COMMENT '用户名',
  `f_online` bigint(20) NOT NULL COMMENT '用户ID',
  `f_photo` varchar(64) DEFAULT NULL,
  `f_signature` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`f_user_id`),
  UNIQUE KEY `T_USER_pk_2` (`f_user_id`),
  KEY `T_USER_f_user_id_index` (`f_online`)
) ENGINE=InnoDB AUTO_INCREMENT=10122 DEFAULT CHARSET=utf8 COMMENT='用户信息表';
