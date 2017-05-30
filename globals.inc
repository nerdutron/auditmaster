<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: globals.inc.sample 13 2006-12-10 13:10:12Z scott                 $ //
//                                                                          //
//  Sanctuary Faith Community Management Software                           //
//  Copyright (C) Authors listed in AUTHORS.txt                             //
//                                                                          //
//  This program is free software; you can redistribute it and/or modify    //
//  it under the terms of the GNU General Public License as published by    //
//  the Free Software Foundation; either version 2 of the License, or       //
//  (at your option) any later version.                                     //
//                                                                          //
//  This program is distributed in the hope that it will be useful,         //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           //
//  GNU General Public License for more details.                            //
//                                                                          //
//  You should have received a copy of the GNU General Public License       //
//  along with this program; if not, write to the Free Software             //
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// define environment 
define("ENV", "dev");
define("REL", "dev");

define("COMPANY_NAME", "ATA Group");
define("SYSTEM_FROM_EMAIL", "audit@tatagroup.com.au");
define("DEFAULT_LOGO", "img/ata_logo.png");

// define paths for php libraries
define("SRC_PATH", "../..");
//define("PHPDB_DIR", SRC_PATH."/phpdb/");
define("PHPLIB_DIR", SRC_PATH."/phplib5/");
define("WWW_DIR", SRC_PATH."/core/www/");

require_once(PHPLIB_DIR."auth.inc");


// The database server that most databases live on
define("DB_SERVER", "localhost");

// databases
define("WEB_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@".DB_SERVER."/WEB_".strtoupper(ENV));
define("MAIL_Q_DB", WEB_DB);
define("PROCESS_DB", WEB_DB);
define("PAGE_CACHE_DB", WEB_DB);

// environment databases
define("ACC_DB_NAME", "ATAGROUP_LIVE");
//define("ACC_DB_NAME", "AUDITMASTER");
//define("OLD_DB_NAME", "ATAG_LATEST");

define("ACC_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@".DB_SERVER."/".ACC_DB_NAME);
//define("DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@".DB_SERVER."/AM_LIVE");
define("DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@".DB_SERVER."/".ACC_DB_NAME);
//define("UPDATED_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@".DB_SERVER."/".ACC_DB_NAME);
define("OLD_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@".DB_SERVER."/".OLD_DB_NAME);

// base paths and functions for accessing simply
define("INCLUDE_PATH", SRC_PATH."/PROJECT_NAME/includes/");
define("OBJECTS_PATH", SRC_PATH."/PROJECT_NAME/objects/");
define("REPORTS_PATH", SRC_PATH."/PROJECT_NAME/reports/");
//define("XSLT_PATH", SRC_PATH."/PROJECT_NAME/xslt/");
//define("WSDL_PATH", SRC_PATH."/PROJECT_NAME/wsdl/");
//define("CHARTING", SRC_PATH."/chart_director/phpchartdir.php");


function ToObjects($Project)
{
	return(str_replace("PROJECT_NAME", $Project, OBJECTS_PATH));
}
function ToIncludes($Project)
{
	return(str_replace("PROJECT_NAME", $Project, INCLUDE_PATH));
}
function ToReports($Project)
{
	return(str_replace("PROJECT_NAME", $Project, REPORTS_PATH));
}
function ToXSLT($Project)
{
	return(str_replace("PROJECT_NAME", $Project, XSLT_PATH));
}
function ToWSDL($Project)
{
	return(str_replace("PROJECT_NAME", $Project, WSDL_PATH));
}


// Other general purpose defines
$PROCESS = array();
$PROCESS["restart_maximum"] = 100;
$PROCESS["load_sensitivity"] = 3;
$PROCESS["every"] = 1;
$PROCESS["delay"] = 10;
$PROCESS["timeout"] = 150;
$PROCESS["description"] = ENV.": ";
$PROCESS["email"] = "scott@mu.com.au";
$PROCESS["pager"] = "";
$PROCESS["log_count"] = 25;

define ('DATE_FORMAT', 'd/m/Y');

?>
