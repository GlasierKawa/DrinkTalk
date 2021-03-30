/*
Date: 2021-03-25 19:16:39
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for T_CHAT_MESSAGE
-- ----------------------------
DROP TABLE IF EXISTS `T_CHAT_MESSAGE`;
CREATE TABLE `T_CHAT_MESSAGE` (
  `f_message_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID',
  `f_senderid` bigint(20) NOT NULL COMMENT '发送者id',
  `f_targetid` bigint(20) NOT NULL COMMENT '接收者id',
  `f_msgcontent` varchar(64) NOT NULL COMMENT '聊天内容',
  `f_sendtime` varchar(64) NOT NULL,
  `f_type` int(11) NOT NULL,
  PRIMARY KEY (`f_message_id`),
  UNIQUE KEY `T_CHAT_MESSAGE_pk_2` (`f_message_id`),
  KEY `f_c_m_key1` (`f_senderid`),
  KEY `f_c_m_key2` (`f_targetid`),
  CONSTRAINT `f_c_m_key2` FOREIGN KEY (`f_targetid`) REFERENCES `T_USER` (`f_user_id`),
  CONSTRAINT `f_c_m_key1` FOREIGN KEY (`f_senderid`) REFERENCES `T_USER` (`f_user_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1331 DEFAULT CHARSET=utf8 COMMENT='消息记录表';
