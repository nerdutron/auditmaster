<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/
require_once(PHPLIB_DIR."cms/cms_item_base.inc");

class cms_text extends cms_item_base
{
	function mce()
	{
		// 0 = no mce
		// 1 = formatted text
		// 2 = basic html
		// 3 = advanced html
		return(0);
	}

	function mode()
	{
		return("ITEM");
	}
}

?>
