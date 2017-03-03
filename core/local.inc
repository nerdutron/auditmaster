<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: local.inc.sample 13 2006-12-10 13:10:12Z scott                   $ //
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

// relational to the bin/ or www/ drectory
require_once("../../globals.inc");
define("OBJECTS_DIR", ToObjects("core"));
define("CONTAINER_PROJECT", "core");

// other defines etc specific to main site here
// require_once(PHPLIB_DIR."tools.inc");
// DiskLogFile("core");
// DiskLogRecord("ALL");
define("SESSION_COOKIE", "SANCTUARY_SESSION");
define("SESSION_EXPIRY", "+ 4 hours");
define("COOKIE_NAME", "SANCTUARY_ACCESS");
define("COOKIE_ENCRYPT", "1");
define("PAGE_CACHE_KEY", COOKIE_NAME);

// file browser definitions
define("FILE_BROWSER", "index.php?Action=file_manager");
define("FILE_DB", ACC_DB);
define("FILE_ALLOW", "");
define("FILE_DENY", "exe|bat");

?>
