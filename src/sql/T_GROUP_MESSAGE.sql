/*
Date: 2021-03-25 19:19:45
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for T_GROUP_MESSAGE
-- ----------------------------
DROP TABLE IF EXISTS `T_GROUP_MESSAGE`;
CREATE TABLE `T_GROUP_MESSAGE` (
  `f_message_id` bigint(20) NOT NULL AUTO_INCREMENT,
  `f_senderid` bigint(20) NOT NULL,
  `f_groupid` bigint(20) NOT NULL,
  `f_msgcontent` varchar(64) DEFAULT NULL,
  `f_sendtime` varchar(64) DEFAULT NULL,
  `f_type` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`f_message_id`),
  KEY `f_g_m_key` (`f_senderid`),
  KEY `f_g_m_key1` (`f_groupid`),
  CONSTRAINT `f_g_m_key` FOREIGN KEY (`f_senderid`) REFERENCES `T_GROUP_MEMBER` (`f_user_id`),
  CONSTRAINT `f_g_m_key1` FOREIGN KEY (`f_groupid`) REFERENCES `T_GROUP` (`f_group_id`)
) ENGINE=InnoDB AUTO_INCREMENT=188 DEFAULT CHARSET=latin1;
