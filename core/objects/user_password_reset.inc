<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: about.inc 8 2006-12-10 05:06:30Z scott                           $ //
//                                                                          //
// 	Object Model															// 
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

require_once(ToObjects("core")."user_maintenance.inc");
require_once(ToIncludes("core")."user_maintenance_data.inc");

class user_password_reset extends user_maintenance
{
	function __construct()
	{
		parent::__construct();

		$this->MakeL("EUserID");

		if ($this->EUserID)
		{
			$this->User->UserPasswordCreate($this->EUserID);
		}

	}

	function PageContent($Message = "")
	{
		if ($this->EUserID)
		{
			parent::PageContent("Password Reset");
		}
		else
			parent::PageContent("Password Reset");
	}
}

?>
