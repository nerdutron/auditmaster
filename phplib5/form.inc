<?php
/*
Copyright: MU Systems Pty Ltd, 2004

Optional Form Elements:

	And all that Malarkey // Trimming form elements

	Please feel free to use this JavaScript file in any way that you like, although please
	keep the comments intact and a link back to http://www.stuffandnonsense.co.uk/archives/trimming_form_fields.html
	would be appreciated.

	If you come up with a stunning design based on this technique, it would be really nice
	if you would post a comment containing a URL on 
	http://www.stuffandnonsense.co.uk/archives/trimming_form_fields.html#Comments 

	Thanks to Brothercake (http://www.brothercake.com/) for his help with the Javascript.
	His fantastic UDM 4 fully-featured and accessible website menu system is a must!
	(http://www.udm4.com/) 

*/

require_once(PHPLIB_DIR."field.inc");

class form
{
	function form($Action, $File = 0, $Extra = "", $Trap = 0)
	{
		$this->TurnOn = "Display optional fields?";
		$this->TurnOff = "Hide optional fields?";
		$this->Form = field::GetFormStart($Action, $File, $Extra, $Trap);
		$this->Fields = "";
		$this->Label = 0;
	}

	function toggle($Mode = "", $Mark = "")
	{
		if (!$Mode)
			$Mode = "show";
		if (!$Mark)
			$Mark = "Marked";
		$this->Form .= "<p id=\"form_intro\"><span class=\"form_req\">".$Mark." fields</span> are required.</p>\n";
		$this->Mode = $Mode;
	}

	function label($Width)
	{
		$this->Label = $Width;
	}

	function toggle_on($Label)
	{
		if (trim($Label))
			$this->TurnOn = $Label;
	}

	function toggle_off($Label)
	{
		if (trim($Label))
			$this->TurnOff = $Label;
	}

	function optional()
	{
		if (fmod(func_num_args(), 3))
			exit("Fatal error: form inputs must be in triples: [field_name], [text_label], [form_data].");

		$this->Fields .= "<div class=\"form_opt\">";
		for ($i = 0;$i<func_num_args(); $i=$i+3)
		{
			$this->Fields .= "<label for=\"".func_get_arg($i)."\">";
			if (func_get_arg($i+1))
				$this->Fields .= func_get_arg($i+1).":";
			else
				$this->Fields .= "&nbsp;";
			$this->Fields .= "</label>";
			if (func_num_args() > ($i+3))
				$this->Fields .= "<span class=\"form_field\">";
			$this->Fields .= func_get_arg($i+2);
			if (func_num_args() > ($i+3))
				$this->Fields .= "</span>";
		}
		$this->Fields .= "</div>";
		if (func_num_args() > 3)
				$this->Fields .= "<div class=\"form_clear\"></div>\n";
	}

	function required()
	{
		if (fmod(func_num_args(), 3))
			exit("Fatal error: form inputs must be in triples: [field_name], [text_label], [form_data].");

		$this->Fields .= "<div class=\"form_req\">";
		for ($i = 0;$i<func_num_args(); $i=$i+3)
		{
			$this->Fields .= "<label for=\"".func_get_arg($i)."\">";
			if (func_get_arg($i+1))
				$this->Fields .= func_get_arg($i+1).":";
			else
				$this->Fields .= "&nbsp;";
			$this->Fields .= "</label>";
			if (func_num_args() > ($i+3))
				$this->Fields .= "<span class=\"form_field\">";
			$this->Fields .= func_get_arg($i+2);
			if (func_num_args() > ($i+3))
				$this->Fields .= "</span>";
		}
		$this->Fields .= "</div>";
		if (func_num_args() > 3)
				$this->Fields .= "<div class=\"form_clear\"></div>\n";
	}

	function text($Text)
	{
		$this->Fields .= $Text;
	}

	function append($Text)
	{
		if (substr($this->Fields, -6, 6) == "</div>")
			$this->Fields = substr($this->Fields, 0, -6).$Text."</div>";
		else
			$this->Fields .= $Text;
	}

	function add_to_group($Label = "", $Extra = "")
	{
		$this->Form .= field::GetFieldsetStart($Label, $Extra);
		$this->Form .= $this->Fields;
		$this->Fields = "";
		$this->Form .= field::GetFieldsetEnd();
	}

	function add_to_form()
	{
		$this->Form .= $this->Fields;
		$this->Fields = "";
	}

	function close()
	{
		$this->add_to_form();
		$this->Form .= field::GetFormEnd();

		if ($this->Mode)
		{
			$this->Form .= "<style>\n";
			$this->Form .= "p#form_intro { margin: 0; }\n";
			$this->Form .= ".form_req { font-weight: bold; }\n";
			if ($this->Mode == "show")
				$this->Form .= ".form_opt { display: block; }\n";
			else
				$this->Form .= ".form_opt { display: none; }\n";
			if ($this->Label)
			{
				$this->Form .= "label { width: ".$this->Label."px; text-align: right; float: left; padding: 0 1em; }\n";
				$this->Form .= ".form_field { float: left; }\n";
				$this->Form .= ".form_clear { clear: both; }\n";
			}
			$this->Form .= "</style>\n";

			$this->Form .= "<script>\n";
			$this->Form .= "window.onload = function()\n";
			$this->Form .= "{\n";
			$this->Form .= "	if(document.getElementById)\n";
			$this->Form .= "	{\n";
			$this->Form .= "		var linkContainer = document.getElementById('form_intro');\n";
			$this->Form .= "		var linebreak = linkContainer.appendChild(document.createElement('br'));\n";
			$this->Form .= "		var toggle = linkContainer.appendChild(document.createElement('a'));\n";
			$this->Form .= "		toggle.href = '#';\n";
			if ($this->Mode == "show")
				$this->Form .= "		toggle.appendChild(document.createTextNode('".$this->TurnOff."'));\n";
			else
				$this->Form .= "		toggle.appendChild(document.createTextNode('".$this->TurnOn."'));\n";
			$this->Form .= "		toggle.onclick = function()\n";
			$this->Form .= "		{\n";
			$this->Form .= "			var linkText = this.firstChild.nodeValue;\n";
			if ($this->Mode == "show")
				$this->Form .= "			this.firstChild.nodeValue = (linkText == '".$this->TurnOff."') ? '".$this->TurnOn."' : '".$this->TurnOff."';\n";
			else
				$this->Form .= "			this.firstChild.nodeValue = (linkText == '".$this->TurnOn."') ? '".$this->TurnOff."' : '".$this->TurnOn."';\n";
			$this->Form .= "			var tmp = document.getElementsByTagName('div');\n";
			$this->Form .= "			for (var i=0;i<tmp.length;i++)\n";
			$this->Form .= "			{\n";
			$this->Form .= "				if(tmp[i].className == 'form_opt')\n";
			$this->Form .= "				{\n";
			if ($this->Mode == "show")
				$this->Form .= "					tmp[i].style.display = (tmp[i].style.display == 'none') ? 'block' : 'none';\n";
			else
				$this->Form .= "					tmp[i].style.display = (tmp[i].style.display == 'block') ? 'none' : 'block';\n";
			$this->Form .= "				}\n";
			$this->Form .= "			}\n";
			$this->Form .= "			return false;\n";
			$this->Form .= "		}\n";
			$this->Form .= "	}\n";
			$this->Form .= "}\n";
			$this->Form .= "</script>\n";
		}
		else
		{
			$this->Form .= "<style>\n";
			$this->Form .= ".form_req { font-weight: bold; }\n";
			if ($this->Label)
			{
				$this->Form .= "label { width: ".$this->Label."px; text-align: right; float: left; padding: 0 1em; }\n";
				$this->Form .= ".form_field { float: left; }\n";
				$this->Form .= ".form_clear { clear: both; }\n";
			}
			$this->Form .= "</style>\n";
		}
		return ($this->Form);
	}
}

?>
