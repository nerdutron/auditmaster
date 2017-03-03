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
-- Table structure for table `audit_notes`
--

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
-- Dumping data for table `audit_notes`
--

LOCK TABLES `audit_notes` WRITE;
/*!40000 ALTER TABLE `audit_notes` DISABLE KEYS */;
/*!40000 ALTER TABLE `audit_notes` ENABLE KEYS */;
UNLOCK TABLES;

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
  PRIMARY KEY (`audit_id`)
) ENGINE=MyISAM AUTO_INCREMENT=18 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `audits`
--

LOCK TABLES `audits` WRITE;
/*!40000 ALTER TABLE `audits` DISABLE KEYS */;
INSERT INTO `audits` VALUES (4,1,1,2,1,'12.00','12.00','2010-06-08','2010-06-29','0000-00-00',''),(5,1,2,5,2,'33.00','44.00','0000-00-00','0000-00-00','0000-00-00',''),(6,1,1,1,3,NULL,NULL,'2010-06-15','0000-00-00','0000-00-00',''),(7,1,1,1,4,'60.00',NULL,'2010-06-07','2010-06-26','0000-00-00',''),(8,1,1,1,5,NULL,NULL,'2010-06-08','0000-00-00','0000-00-00',''),(9,1,0,1,6,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING'),(10,1,0,1,7,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING'),(11,1,0,1,8,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING'),(12,1,0,1,7,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING'),(13,1,0,1,4,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING'),(14,1,0,1,2,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING'),(15,1,0,1,4,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING'),(16,3,0,1,1,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING'),(17,3,0,1,3,NULL,NULL,'0000-00-00','0000-00-00','0000-00-00','PENDING');
/*!40000 ALTER TABLE `audits` ENABLE KEYS */;
UNLOCK TABLES;

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
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `contacts`
--

LOCK TABLES `contacts` WRITE;
/*!40000 ALTER TABLE `contacts` DISABLE KEYS */;
INSERT INTO `contacts` VALUES (1,'Jones','Bob','','2342134','809890809','0423423432','bob@jones.com.au','Bossman',1),(2,'Jones','Michelle','','242342','','02342342342','michelle@yahoo.com.au','Wife',1),(3,'Jones','Bob','','','','','rod@thecoles.id.au','',3);
/*!40000 ALTER TABLE `contacts` ENABLE KEYS */;
UNLOCK TABLES;

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
  `file_data` longtext,
  `audit_id` int(11) NOT NULL,
  PRIMARY KEY (`file_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `files`
--

LOCK TABLES `files` WRITE;
/*!40000 ALTER TABLE `files` DISABLE KEYS */;
/*!40000 ALTER TABLE `files` ENABLE KEYS */;
UNLOCK TABLES;

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
  PRIMARY KEY (`note_id`)
) ENGINE=MyISAM AUTO_INCREMENT=33 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `notes`
--

LOCK TABLES `notes` WRITE;
/*!40000 ALTER TABLE `notes` DISABLE KEYS */;
INSERT INTO `notes` VALUES (26,1,'asdfasfaZCZCx','2010-06-08',NULL),(27,1,'ccvvcava','2010-06-08',NULL),(28,1,'ccvvcava','2010-06-08',NULL),(29,1,'ccvvcava','2010-06-08',NULL),(30,3,'A note here','2010-06-11',NULL),(31,3,'Another note here','2010-06-11',NULL),(32,3,'Another note here','2010-06-11',NULL);
/*!40000 ALTER TABLE `notes` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `operator_status`
--

LOCK TABLES `operator_status` WRITE;
/*!40000 ALTER TABLE `operator_status` DISABLE KEYS */;
/*!40000 ALTER TABLE `operator_status` ENABLE KEYS */;
UNLOCK TABLES;

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
  `address_1` varchar(128) DEFAULT NULL,
  `address_2` varchar(128) DEFAULT NULL,
  `suburb` varchar(64) DEFAULT NULL,
  `postcode` varchar(4) DEFAULT NULL,
  `state_id` int(11) DEFAULT NULL,
  `operator_status_id` int(11) DEFAULT NULL,
  `state` varchar(4) DEFAULT NULL,
  PRIMARY KEY (`operator_id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `operators`
--

LOCK TABLES `operators` WRITE;
/*!40000 ALTER TABLE `operators` DISABLE KEYS */;
INSERT INTO `operators` VALUES (1,'Test 1','Trading Dummy','23423423423','Bob',1,'','','23423','234234','0000-00-00','2 This St','','Bayview Heights','4234',NULL,NULL,'QLD'),(2,'','','','',1,'','','','','0000-00-00','','','','',NULL,NULL,''),(3,'Bobs Deliveries','Bobs Deliveries','543523453','',1,'','','','','0000-00-00','','','','',NULL,NULL,'QLD'),(4,'Jones and Co','Big Rigs','','',1,'','','','','0000-00-00','','','','',NULL,NULL,'QLD');
/*!40000 ALTER TABLE `operators` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `regions`
--

LOCK TABLES `regions` WRITE;
/*!40000 ALTER TABLE `regions` DISABLE KEYS */;
/*!40000 ALTER TABLE `regions` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `scheme_signup`
--

LOCK TABLES `scheme_signup` WRITE;
/*!40000 ALTER TABLE `scheme_signup` DISABLE KEYS */;
/*!40000 ALTER TABLE `scheme_signup` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `schemes`
--

DROP TABLE IF EXISTS `schemes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `schemes` (
  `scheme_id` int(11) NOT NULL AUTO_INCREMENT,
  `scheme_name` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`scheme_id`)
) ENGINE=MyISAM AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `schemes`
--

LOCK TABLES `schemes` WRITE;
/*!40000 ALTER TABLE `schemes` DISABLE KEYS */;
INSERT INTO `schemes` VALUES (1,'Trucksafe'),(2,'Truckcare'),(3,'Maintenance (NHVAS)'),(4,'Mass (NHVAS)'),(5,'Fatigue (NHVAS)'),(6,'W A Maintenance'),(7,'W A Fatigue'),(8,'Q-Ride');
/*!40000 ALTER TABLE `schemes` ENABLE KEYS */;
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
) ENGINE=MyISAM AUTO_INCREMENT=32 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `security_logging`
--

LOCK TABLES `security_logging` WRITE;
/*!40000 ALTER TABLE `security_logging` DISABLE KEYS */;
INSERT INTO `security_logging` VALUES (1,'rod',0,'127.0.0.1','2010-04-10 12:20:35','LOGIN'),(2,'rod',0,'127.0.0.1','2010-04-10 12:21:44','LOGIN'),(3,'rod',1,'127.0.0.1','2010-04-10 12:26:41','USER'),(4,'rod',1,'127.0.0.1','2010-04-10 13:04:28','USER'),(5,'rod',1,'127.0.0.1','2010-04-12 09:33:01','USER'),(6,'rod',1,'127.0.0.1','2010-04-12 09:56:28','USER'),(7,'rod',1,'127.0.0.1','2010-04-12 09:57:01','USER'),(8,'rod',1,'127.0.0.1','2010-04-12 09:57:10','USER'),(9,'rod',1,'127.0.0.1','2010-04-12 09:57:25','USER'),(10,'rod',1,'127.0.0.1','2010-04-12 09:58:28','USER'),(11,'rod',1,'127.0.0.1','2010-04-12 11:26:53','SUCCESS'),(12,'rod',1,'127.0.0.1','2010-05-17 12:13:28','SUCCESS'),(13,'rod',1,'127.0.0.1','2010-05-19 12:37:09','SUCCESS'),(14,'rod',1,'127.0.0.1','2010-05-19 21:32:03','SUCCESS'),(15,'rod',1,'127.0.0.1','2010-05-21 10:27:52','SUCCESS'),(16,'rod',1,'127.0.0.1','2010-05-24 09:39:56','SUCCESS'),(17,'rod',1,'127.0.0.1','2010-05-25 11:14:35','PASSWORD'),(18,'rod',1,'127.0.0.1','2010-05-25 11:14:42','SUCCESS'),(19,'rod',1,'127.0.0.1','2010-05-25 20:49:48','SUCCESS'),(20,'rod',1,'127.0.0.1','2010-05-27 13:26:17','SUCCESS'),(21,'rod',1,'127.0.0.1','2010-06-07 22:23:00','SUCCESS'),(22,'rod',1,'127.0.0.1','2010-06-08 10:39:06','SUCCESS'),(23,'rod',1,'127.0.0.1','2010-06-09 11:06:42','SUCCESS'),(24,'rod',1,'127.0.0.1','2010-06-09 11:33:14','SUCCESS'),(25,'rod',1,'127.0.0.1','2010-06-10 09:52:48','SUCCESS'),(26,'rod',1,'127.0.0.1','2010-06-11 10:39:10','SUCCESS'),(27,'rod',1,'127.0.0.1','2010-06-14 11:34:32','SUCCESS'),(28,'rod',1,'127.0.0.1','2010-06-14 17:19:16','SUCCESS'),(29,'rod',1,'127.0.0.1','2010-06-15 10:04:45','SUCCESS'),(30,'rod',1,'127.0.0.1','2010-06-15 21:52:48','SUCCESS'),(31,'rod',1,'127.0.0.1','2010-06-16 10:20:44','SUCCESS');
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
-- Dumping data for table `signup_status`
--

LOCK TABLES `signup_status` WRITE;
/*!40000 ALTER TABLE `signup_status` DISABLE KEYS */;
/*!40000 ALTER TABLE `signup_status` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `stages`
--

DROP TABLE IF EXISTS `stages`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `stages` (
  `stage_id` int(11) NOT NULL AUTO_INCREMENT,
  `description` varchar(128) DEFAULT NULL,
  `months` int(5) DEFAULT NULL,
  PRIMARY KEY (`stage_id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `stages`
--

LOCK TABLES `stages` WRITE;
/*!40000 ALTER TABLE `stages` DISABLE KEYS */;
INSERT INTO `stages` VALUES (1,'Entry',0),(2,'5 mth Compliance',6),(3,'Annual Compliance',12),(4,'18 mth Re-accreditation',18),(5,'2 year Re-accreditation',24);
/*!40000 ALTER TABLE `stages` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `states`
--

DROP TABLE IF EXISTS `states`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `states` (
  `state_code` varchar(3) NOT NULL,
  `state` varchar(16) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `states`
--

LOCK TABLES `states` WRITE;
/*!40000 ALTER TABLE `states` DISABLE KEYS */;
INSERT INTO `states` VALUES ('QLD','Queensland'),('NSW','New South Wales'),('VIC','Victoria'),('NT','Northern Territo'),('SA','South Australia'),('TAS','Tasmania'),('WA','Western Australi');
/*!40000 ALTER TABLE `states` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `suburbs`
--

DROP TABLE IF EXISTS `suburbs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `suburbs` (
  `suburb_id` int(11) NOT NULL AUTO_INCREMENT,
  `suburb` varchar(64) DEFAULT NULL,
  `state` char(3) DEFAULT NULL,
  `postcode` char(4) DEFAULT NULL,
  PRIMARY KEY (`suburb_id`)
) ENGINE=MyISAM AUTO_INCREMENT=51 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `suburbs`
--

LOCK TABLES `suburbs` WRITE;
/*!40000 ALTER TABLE `suburbs` DISABLE KEYS */;
INSERT INTO `suburbs` VALUES (1,'AUSTRALIAN NATIONAL UNIVERSITY','ACT','200'),(2,'BARTON','ACT','221'),(3,'DARWIN','NT','800'),(4,'DARWIN','NT','801'),(5,'PARAP','NT','804'),(6,'ALAWA','NT','810'),(7,'BRINKIN','NT','810'),(8,'CASUARINA','NT','810'),(9,'COCONUT GROVE','NT','810'),(10,'JINGILI','NT','810'),(11,'LEE POINT','NT','810'),(12,'MILLNER','NT','810'),(13,'MOIL','NT','810'),(14,'NAKARA','NT','810'),(15,'NIGHTCLIFF','NT','810'),(16,'RAPID CREEK','NT','810'),(17,'TIWI','NT','810'),(18,'WAGAMAN','NT','810'),(19,'WANGURI','NT','810'),(20,'CASUARINA','NT','811'),(21,'ANULA','NT','812'),(22,'KARAMA','NT','812'),(23,'LEANYER','NT','812'),(24,'MALAK','NT','812'),(25,'MARRARA','NT','812'),(26,'NORTHLAKES','NT','812'),(27,'SANDERSON','NT','812'),(28,'WOODLEIGH GARDENS','NT','812'),(29,'WULAGI','NT','812'),(30,'SANDERSON','NT','813'),(31,'NIGHTCLIFF','NT','814'),(32,'CHARLES DARWIN UNIVERSITY','NT','815'),(33,'BAGOT','NT','820'),(34,'BAYVIEW','NT','820'),(35,'CHARLES DARWIN','NT','820'),(36,'COONAWARRA','NT','820'),(37,'CULLEN BAY','NT','820'),(38,'DARWIN DC','NT','820'),(39,'DARWIN INTERNATIONAL AIRPORT','NT','820'),(40,'DARWIN MC','NT','820'),(41,'EAST POINT','NT','820'),(42,'FANNIE BAY','NT','820'),(43,'LARRAKEYAH','NT','820'),(44,'LUDMILLA','NT','820'),(45,'PARAP','NT','820'),(46,'RAAF BASE DARWIN','NT','820'),(47,'STUART PARK','NT','820'),(48,'THE GARDENS','NT','820'),(49,'THE NARROWS','NT','820'),(50,'WINNELLIE','NT','820');
/*!40000 ALTER TABLE `suburbs` ENABLE KEYS */;
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
) ENGINE=MyISAM AUTO_INCREMENT=23 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_session`
--

LOCK TABLES `user_session` WRITE;
/*!40000 ALTER TABLE `user_session` DISABLE KEYS */;
INSERT INTO `user_session` VALUES (1,'iiclogvkl3kid7ppith3doucwzahgykp7ngl',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-04-10 22:20:43','2010-04-10 16:37:09',NULL),(2,'59n4rft63kyysm494etjdgwgr4rkeu9p9mj',1,'','2010-04-12 10:02:55','2010-04-12 09:21:25',NULL),(3,'ksf83qosspo18qbni1igath7kkon9vejtjhn',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-04-12 16:35:37','2010-04-12 11:26:53',NULL),(4,'2fv4u4qqb2epktlw0e0ypluzngxv3w2vhn51',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-05-17 16:13:28','2010-05-17 12:13:28',NULL),(5,'1gjfh3twql7vp2900zpsd1dd64ld3oiuu70h',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-05-19 16:54:33','2010-05-19 12:37:09',NULL),(6,'jlx2f66csp3owq8cw10wld23l3vy1f9b7cjc',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-05-20 01:37:21','2010-05-19 21:32:03',NULL),(7,'hj7p1cq78e2eu7rb1epm3tzfcxxbi3ipcfkj',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-05-21 18:50:50','2010-05-21 10:27:52',NULL),(8,'8ptac4ku0zhmljbhb2rppfueg4hysocqjw62',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-05-24 20:27:51','2010-05-24 09:39:56',NULL),(9,'y7ykmqlw8jva8vc6maxelnsk77vdwwjk9nb2',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-05-25 15:16:23','2010-05-25 11:14:42',NULL),(10,'w6fa4w5d9c5hrp04z6pg8y2bksthlsz7557z',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-05-26 02:09:18','2010-05-25 20:49:48',NULL),(11,'u64635bkapbt8x4fyjuxq5qlpnmo3by3ns0w',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-05-27 17:26:17','2010-05-27 13:26:17',NULL),(12,'y24y2v4qlvl4ot9sonocdur6qy54fltju47m',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-08 06:43:02','2010-06-07 22:23:00',NULL),(13,'jqvk4u8kpqxhmrddttwmhz4d4kitnx2dt3nn',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-08 17:13:05','2010-06-08 10:39:06',NULL),(14,'sifzver1qa0kpzoxkpsx9zwvpjez6pdoxytj',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-09 15:31:32','2010-06-09 11:06:42',NULL),(15,'y6ieqxdphvnc3ackgnt7ej4itkbj0ht4th8a',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-09 15:46:44','2010-06-09 11:33:14',NULL),(16,'ikybkgh40jl7wi2yffuo78rq6nwyes22j6k9',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-11 03:18:40','2010-06-10 09:52:48',NULL),(17,'oocrx0nrg7xpz04o7lkkvp96bfgva2xogz5j',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-11 15:58:44','2010-06-11 10:39:10',NULL),(18,'ab7y8lip5rr111ft2nibrawbffchoeqovnc9',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-14 16:15:29','2010-06-14 11:34:32',NULL),(19,'od0lrunnj7wofrbd6bdnohzs99bacn06q6hn',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-15 01:05:48','2010-06-14 17:19:16',NULL),(20,'5quj2w6w3qja8hujgfbnal4p8zh8iqvemgnv',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-15 14:37:32','2010-06-15 10:04:45',NULL),(21,'ton5x8wfq0lfitjmg40wejmurrzway0ucu5f',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-16 03:39:19','2010-06-15 21:52:48',NULL),(22,'v2lral44z3vqa9t1xcc3sme8si05nk98sk5t',1,'a:3:{s:7:\"_LogoID\";N;s:11:\"_BackGround\";N;s:11:\"_ForeGround\";N;}','2010-06-16 14:41:53','2010-06-16 10:20:44',NULL);
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
  `auditor` char(1) DEFAULT NULL,
  PRIMARY KEY (`user_id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (1,1,'','Rod','rod','iLMd5NTTivfyM','rod@thecoles.id.au',2,0,'35234','2345234','3424234','2010-04-12','2010-04-10 12:06:52','Y'),(2,1,'','Test','test','','test@test.com',1,0,'35234','2345234','3424234','2010-04-12','0000-00-00 00:00:00','Y');
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

-- Dump completed on 2010-06-16 16:17:01
