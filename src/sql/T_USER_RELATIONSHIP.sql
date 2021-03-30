/*
Date: 2021-03-25 19:20:13
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for T_USER_RELATIONSHIP
-- ----------------------------
DROP TABLE IF EXISTS `T_USER_RELATIONSHIP`;
CREATE TABLE `T_USER_RELATIONSHIP` (
  `f_user_id1` bigint(20) NOT NULL COMMENT '第一个用户id',
  `f_user_id2` bigint(20) NOT NULL COMMENT '第二个用户id',
  `f_message_num` bigint(20) DEFAULT '0',
  UNIQUE KEY `T_USER_RELATIONSHIP_pk` (`f_user_id1`,`f_user_id2`),
  KEY `f_f_key1` (`f_user_id2`),
  CONSTRAINT `f_f_key` FOREIGN KEY (`f_user_id1`) REFERENCES `T_USER` (`f_user_id`),
  CONSTRAINT `f_f_key1` FOREIGN KEY (`f_user_id2`) REFERENCES `T_USER` (`f_user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户关系表';
