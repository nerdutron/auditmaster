<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/
require_once(PHPLIB_DIR."cms/cms_list_base.inc");

class cms_faq extends cms_list_base
{
	function mce()
	{
		// 0 = no mce
		// 1 = formatted text
		// 2 = basic html
		// 3 = advanced html
		return(2);
	}

	function mode()
	{
		return("LIST");
	}

	function order()
	{
		return ("SEQ");
	}

	function when()
	{
		return ("l jS F 'y");
	}

	function control()
	{
		return(array("CMSData[Yr]", "CMSData[Mo]", "CMSData[Dy]"));
	}

	function edit($Content, $Item = 0)
	{
		cms_list_base::edit($this->Control(), $Content, $Item);
	}

	function _get($Status, $Content, $Item = 0)
	{
		return(cms_list_base::_get($this->order(), $Status, $Content, $Item));
	}
}

?>
