-- MySQL dump 10.13  Distrib 5.1.37, for apple-darwin8.11.1 (i386)
--
-- Host: localhost    Database: OM
-- ------------------------------------------------------
-- Server version	5.1.37

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `admin_levels`
--

DROP TABLE IF EXISTS `admin_levels`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `admin_levels` (
  `admin_id` int(4) NOT NULL,
  `code` varchar(8) NOT NULL,
  `description` varchar(64) NOT NULL,
  PRIMARY KEY (`admin_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `admin_levels`
--

LOCK TABLES `admin_levels` WRITE;
/*!40000 ALTER TABLE `admin_levels` DISABLE KEYS */;
INSERT INTO `admin_levels` VALUES (1,'USER_ORD','Normal User'),(2,'USER_POW','Power User');
/*!40000 ALTER TABLE `admin_levels` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `security_logging`
--

DROP TABLE IF EXISTS `security_logging`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `security_logging` (
  `event_id` int(11) NOT NULL AUTO_INCREMENT,
  `user_name` varchar(32) NOT NULL,
  `user_id` int(11) NOT NULL DEFAULT '0',
  `ip_address` varchar(19) NOT NULL,
  `recorded` datetime NOT NULL,
  `reason` varchar(24) NOT NULL,
  PRIMARY KEY (`event_id`),
  KEY `idx_user` (`user_name`,`recorded`),
  KEY `idx_ip` (`ip_address`,`recorded`)
) ENGINE=MyISAM AUTO_INCREMENT=12 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `security_logging`
--

LOCK TABLES `security_logging` WRITE;
/*!40000 ALTER TABLE `security_logging` DISABLE KEYS */;
INSERT INTO `security_logging` VALUES (1,'rod',0,'127.0.0.1','2010-04-10 12:20:35','LOGIN'),(2,'rod',0,'127.0.0.1','2010-04-10 12:21:44','LOGIN'),(3,'rod',1,'127.0.0.1','2010-04-10 12:26:41','USER'),(4,'rod',1,'127.0.0.1','2010-04-10 13:04:28','USER'),(5,'rod',1,'127.0.0.1','2010-04-12 09:33:01','USER'),(6,'rod',1,'127.0.0.1','2010-04-12 09:56:28','USER'),(7,'rod',1,'127.0.0.1','2010-04-12 09:57:01','USER'),(8,'rod',1,'127.0.0.1','2010-04-12 09:57:10','USER'),(9,'rod',1,'127.0.0.1','2010-04-12 09:57:25','USER'),(10,'rod',1,'127.0.0.1','2010-04-12 09:58:28','USER'),(11,'rod',1,'127.0.0.1','2010-04-12 11:26:53','SUCCESS');
/*!40000 ALTER TABLE `security_logging` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `security_public`
--

DROP TABLE IF EXISTS `security_public`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `security_public` (
  `user_id` int(11) NOT NULL AUTO_INCREMENT,
  `account_id` int(11) NOT NULL DEFAULT '0',
  `effective_account_id` int(11) NOT NULL DEFAULT '0',
  `active` tinyint(4) NOT NULL DEFAULT '0',
  `os_key` varchar(32) NOT NULL DEFAULT '',
  `name` varchar(128) NOT NULL DEFAULT '',
  `login` varchar(16) NOT NULL DEFAULT '',
  `password` varchar(24) NOT NULL DEFAULT '',
  `email` varchar(128) NOT NULL,
  `user_type` tinyint(4) NOT NULL DEFAULT '0',
  `logins_allowed` tinyint(4) NOT NULL DEFAULT '1',
  `unrestricted_ip` char(1) NOT NULL DEFAULT 'N',
  `profile_id` int(11) NOT NULL DEFAULT '0',
  `user_code_1` varchar(32) DEFAULT NULL,
  `user_code_2` varchar(32) DEFAULT NULL,
  `user_phone` varchar(64) NOT NULL DEFAULT '',
  `user_fax` varchar(64) NOT NULL DEFAULT '',
  `user_mobile` varchar(64) NOT NULL DEFAULT '',
  `created_date` date NOT NULL DEFAULT '0000-00-00',
  `last_login_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `security_public`
--

LOCK TABLES `security_public` WRITE;
/*!40000 ALTER TABLE `security_public` DISABLE KEYS */;
/*!40000 ALTER TABLE `security_public` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `service_objects_permissions`
--

DROP TABLE IF EXISTS `service_objects_permissions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `service_objects_permissions` (
  `entity_type` varchar(8) NOT NULL,
  `entity_id` int(11) NOT NULL,
  `secure_object` varchar(64) NOT NULL,
  `type` char(5) NOT NULL DEFAULT 'ALLOW',
  PRIMARY KEY (`entity_type`,`entity_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `service_objects_permissions`
--

LOCK TABLES `service_objects_permissions` WRITE;
/*!40000 ALTER TABLE `service_objects_permissions` DISABLE KEYS */;
/*!40000 ALTER TABLE `service_objects_permissions` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user_session`
--

DROP TABLE IF EXISTS `user_session`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_session` (
  `session_id` int(11) NOT NULL AUTO_INCREMENT,
  `os_key` varchar(36) NOT NULL DEFAULT '',
  `user_id` int(11) DEFAULT NULL,
  `session_data` text,
  `session_expires` datetime DEFAULT NULL,
  `session_start` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `session_values` text,
  PRIMARY KEY (`session_id`),
  KEY `idx_os_key` (`os_key`)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_session`
--

LOCK TABLES `user_session` WRITE;
/*!40000 ALTER TABLE `user_session` DISABLE KEYS */;
INSERT INTO `user_session` VALUES (1,'iiclogvkl3kid7ppith3doucwzahgykp7ngl',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-04-10 22:20:43','2010-04-10 16:37:09',NULL),(2,'59n4rft63kyysm494etjdgwgr4rkeu9p9mj',1,'','2010-04-12 10:02:55','2010-04-12 09:21:25',NULL),(3,'ksf83qosspo18qbni1igath7kkon9vejtjhn',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-04-12 16:35:37','2010-04-12 11:26:53',NULL);
/*!40000 ALTER TABLE `user_session` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `user_session_current`
--

DROP TABLE IF EXISTS `user_session_current`;
/*!50001 DROP VIEW IF EXISTS `user_session_current`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `user_session_current` (
  `session_id` int(11),
  `os_key` varchar(36),
  `user_id` int(11),
  `session_data` text,
  `session_expires` datetime,
  `session_start` datetime,
  `session_values` text
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `user_id` int(11) NOT NULL AUTO_INCREMENT,
  `active` tinyint(4) NOT NULL DEFAULT '0',
  `os_key` varchar(32) NOT NULL DEFAULT '',
  `name` varchar(128) NOT NULL DEFAULT '',
  `login` varchar(16) NOT NULL DEFAULT '',
  `password` varchar(24) NOT NULL DEFAULT '',
  `email` varchar(128) NOT NULL,
  `user_type` tinyint(4) NOT NULL DEFAULT '0',
  `logins_allowed` tinyint(4) NOT NULL DEFAULT '1',
  `user_phone` varchar(64) NOT NULL DEFAULT '',
  `user_fax` varchar(64) NOT NULL DEFAULT '',
  `user_mobile` varchar(64) NOT NULL DEFAULT '',
  `created_date` date NOT NULL DEFAULT '0000-00-00',
  `last_login_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (1,1,'','Rod','rod','iLMd5NTTivfyM','rod@thecoles.id.au',2,0,'35234','2345234','3424234','2010-04-12','2010-04-10 12:06:52'),(2,1,'','Test','test','','test@test.com',1,0,'35234','2345234','3424234','2010-04-12','0000-00-00 00:00:00');
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Final view structure for view `user_session_current`
--

/*!50001 DROP TABLE `user_session_current`*/;
/*!50001 DROP VIEW IF EXISTS `user_session_current`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = latin1 */;
/*!50001 SET character_set_results     = latin1 */;
/*!50001 SET collation_connection      = latin1_swedish_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `user_session_current` AS select `user_session`.`session_id` AS `session_id`,`user_session`.`os_key` AS `os_key`,`user_session`.`user_id` AS `user_id`,`user_session`.`session_data` AS `session_data`,`user_session`.`session_expires` AS `session_expires`,`user_session`.`session_start` AS `session_start`,`user_session`.`session_values` AS `session_values` from `user_session` where (`user_session`.`session_expires` > now()) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2010-04-13  9:13:58
