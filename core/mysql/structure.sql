-- MySQL dump 10.13  Distrib 5.1.37, for apple-darwin8.11.1 (i386)
--
-- Host: localhost    Database: AUDITMASTER
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

DROP TABLE IF EXISTS `audit_notes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `audit_notes` (
  `audit_note_id` int(11) NOT NULL AUTO_INCREMENT,
  `audit_id` int(11) NOT NULL,
  `note` text,
  PRIMARY KEY (`audit_note_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `audits`
--

DROP TABLE IF EXISTS `audits`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `audits` (
  `audit_id` int(11) NOT NULL AUTO_INCREMENT,
  `operator_id` int(11) NOT NULL,
  `auditor_id` int(11) NOT NULL,
  `stage_id` int(11) NOT NULL,
  `scheme_id` int(11) NOT NULL,
  `cost` decimal(16,2) DEFAULT NULL,
  `paid` decimal(16,2) DEFAULT NULL,
  `date_due` date DEFAULT NULL,
  `date_completed` date DEFAULT NULL,
  `date_closed` date DEFAULT NULL,
  `audit_status` varchar(32) DEFAULT NULL,
  `net_cost` decimal(16,2) DEFAULT NULL,
  PRIMARY KEY (`audit_id`)
) ENGINE=MyISAM AUTO_INCREMENT=15745 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `contacts`
--

DROP TABLE IF EXISTS `contacts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `contacts` (
  `contact_id` int(11) NOT NULL AUTO_INCREMENT,
  `surname` varchar(64) DEFAULT NULL,
  `firstname` varchar(64) DEFAULT NULL,
  `nickname` varchar(64) DEFAULT NULL,
  `phone` varchar(32) DEFAULT NULL,
  `fax` varchar(32) DEFAULT NULL,
  `mobile` varchar(32) DEFAULT NULL,
  `email` varchar(64) DEFAULT NULL,
  `position` varchar(64) DEFAULT NULL,
  `operator_id` int(11) NOT NULL,
  PRIMARY KEY (`contact_id`)
) ENGINE=MyISAM AUTO_INCREMENT=5067 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `file_links`
--

DROP TABLE IF EXISTS `file_links`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `file_links` (
  `file_id` int(11) NOT NULL,
  `audit_id` int(11) NOT NULL,
  `operator_id` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `files`
--

DROP TABLE IF EXISTS `files`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `files` (
  `file_id` int(11) NOT NULL AUTO_INCREMENT,
  `file_name` varchar(128) DEFAULT NULL,
  `file_mime` varchar(128) DEFAULT NULL,
  `file_size` int(11) DEFAULT NULL,
  `loaded` datetime DEFAULT NULL,
  `description` varchar(128) DEFAULT NULL,
  `file_data` longblob,
  `operator_id` int(11) NOT NULL,
  PRIMARY KEY (`file_id`)
) ENGINE=MyISAM AUTO_INCREMENT=35 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `notes`
--

DROP TABLE IF EXISTS `notes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `notes` (
  `note_id` int(11) NOT NULL AUTO_INCREMENT,
  `operator_id` int(11) NOT NULL,
  `note` text,
  `note_date` date DEFAULT NULL,
  `audit_id` int(11) DEFAULT NULL,
  `user_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`note_id`)
) ENGINE=MyISAM AUTO_INCREMENT=15672 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `operator_schemes`
--

DROP TABLE IF EXISTS `operator_schemes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `operator_schemes` (
  `opscheme_id` int(11) NOT NULL AUTO_INCREMENT,
  `operator_id` int(11) NOT NULL,
  `scheme_id` int(11) NOT NULL,
  `stage` int(11) DEFAULT NULL,
  `start_date` date DEFAULT NULL,
  `expiry_date` date DEFAULT NULL,
  PRIMARY KEY (`opscheme_id`)
) ENGINE=MyISAM AUTO_INCREMENT=5589 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `operator_status`
--

DROP TABLE IF EXISTS `operator_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `operator_status` (
  `operator_status_id` int(11) NOT NULL AUTO_INCREMENT,
  `status` int(11) DEFAULT NULL,
  PRIMARY KEY (`operator_status_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `operators`
--

DROP TABLE IF EXISTS `operators`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `operators` (
  `operator_id` int(11) NOT NULL AUTO_INCREMENT,
  `legal_name` varchar(255) DEFAULT NULL,
  `trading_name` varchar(255) DEFAULT NULL,
  `abn` varchar(16) DEFAULT NULL,
  `owners_name` varchar(155) DEFAULT NULL,
  `created_by` int(11) DEFAULT NULL,
  `email` varchar(128) DEFAULT NULL,
  `website` varchar(128) DEFAULT NULL,
  `phone` varchar(32) DEFAULT NULL,
  `fax` varchar(32) DEFAULT NULL,
  `last_audit_date` date DEFAULT NULL,
  `setup_by` int(11) DEFAULT NULL,
  `address_1` varchar(128) DEFAULT NULL,
  `address_2` varchar(128) DEFAULT NULL,
  `suburb` varchar(64) DEFAULT NULL,
  `postcode` varchar(4) DEFAULT NULL,
  `operator_status_id` int(11) DEFAULT NULL,
  `state` varchar(4) DEFAULT NULL,
  `customer_number` varchar(32) DEFAULT NULL,
  `postal_address_1` varchar(128) DEFAULT NULL,
  `postal_address_2` varchar(128) DEFAULT NULL,
  `postal_suburb` varchar(64) DEFAULT NULL,
  `postal_state` varchar(4) DEFAULT NULL,
  `postal_postcode` varchar(4) DEFAULT NULL,
  PRIMARY KEY (`operator_id`)
) ENGINE=MyISAM AUTO_INCREMENT=4068 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `regions`
--

DROP TABLE IF EXISTS `regions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `regions` (
  `region_id` int(11) NOT NULL AUTO_INCREMENT,
  `region_name` varchar(64) DEFAULT NULL,
  `start_postcode` char(4) DEFAULT NULL,
  `end_postcode` char(4) DEFAULT NULL,
  PRIMARY KEY (`region_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `scheme_signup`
--

DROP TABLE IF EXISTS `scheme_signup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `scheme_signup` (
  `scheme_signup_id` int(11) NOT NULL AUTO_INCREMENT,
  `operator_id` int(11) NOT NULL,
  `scheme_id` int(11) NOT NULL,
  `stage_id` int(11) NOT NULL,
  `start_date` date DEFAULT NULL,
  `create_date` date DEFAULT NULL,
  `signup_status` int(11) DEFAULT NULL,
  PRIMARY KEY (`scheme_signup_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

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
) ENGINE=MyISAM AUTO_INCREMENT=74 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;


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
-- Table structure for table `signup_status`
--

DROP TABLE IF EXISTS `signup_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `signup_status` (
  `signup_status_id` int(11) NOT NULL AUTO_INCREMENT,
  `signup_status` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`signup_status_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;


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
) ENGINE=MyISAM AUTO_INCREMENT=65 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

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

-- Dump completed on 2010-08-13 23:00:32
