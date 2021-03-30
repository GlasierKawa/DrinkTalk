/*
Date: 2021-03-25 19:19:38
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for T_GROUP_MEMBER
-- ----------------------------
DROP TABLE IF EXISTS `T_GROUP_MEMBER`;
CREATE TABLE `T_GROUP_MEMBER` (
  `f_group_id` bigint(20) NOT NULL,
  `f_user_id` bigint(20) NOT NULL,
  `f_msg_num` bigint(20) DEFAULT '0',
  PRIMARY KEY (`f_group_id`,`f_user_id`),
  KEY `f_g_m_fukey` (`f_user_id`),
  CONSTRAINT `f_g_m_fkey` FOREIGN KEY (`f_group_id`) REFERENCES `T_GROUP` (`f_group_id`),
  CONSTRAINT `f_g_m_fukey` FOREIGN KEY (`f_user_id`) REFERENCES `T_USER` (`f_user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
