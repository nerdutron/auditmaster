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

require_once(ToObjects("core")."secure.inc");
require_once(ToIncludes("audit")."operator.inc");

class next_operator extends secure
{
	function __construct()
	{
		parent::__construct();

		$this->MakeL("OperatorID", "Direction");

		switch ($this->Direction)
		{
			case "Prev":
				$NewOpID = operator::GetPrevID($this->OperatorID);
				break;
			case "Next":
				$NewOpID = operator::GetNextID($this->OperatorID);
		}

		// If there isn't a next id stay on the same screen
		if (!$NewOpID)
			$NewOpID = $this->OperatorID;

		$this->Redir($this->URL("audit|audit_main", array("OperatorID" => $NewOpID)));
	}
}

?>
