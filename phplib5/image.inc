<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

// Class to facilitate appropriate long descriptions for AAA compatibility
//
// Copyright MU Systems Pty Ltd 2003
//
// Author: Murray Shields        Written: 2/10/2003
//

class Image
{
	function Image($DataSource, $Paths = "", $Relational = "")
	{
		if (!is_array($Paths))
			$Paths = array();
		$this->DataSource = $DataSource;
		$this->Paths = $Paths;
		$this->Relational = $Relational;
		$this->LoadImageData();
	}

	function LongDesc($Image)
	{
		$Image = base64_decode(urldecode($Image));
		echo nl2br(base64_decode($this->DS[$Image]["DSC"]));
	}

	function GetD($SRC)
	{
		$D = "";
		if ($this->DS[$SRC]["DSC"])
			$D .= "<span class=\"d\">[<a href=\"d.htm?Image=".urlencode(base64_encode($SRC))."\">D</a>]</span>";
		return($D);
	}

	function ShowD($SRC)
	{
		echo $this->GetD($SRC);
	}

	function Get($SRC, $Extra = "")
	{
		$D = "";

		if (!is_file($SRC))
			return;
		$IMG  = "";
		$IMG .= "<img src=\"";
		$IMG .= $SRC;
		$IMG .= "\"";

//		if ($this->DS[$SRC]["WID"])
//			$IMG .= " width=".$this->DS[$SRC]["WID"];
//		if ($this->DS[$SRC]["HEI"])
//			$IMG .= " height=".$this->DS[$SRC]["HEI"];
		if ($this->DS[$SRC]["ALT"])
			$IMG .= " alt=\"".base64_decode($this->DS[$SRC]["ALT"])."\"";
		if ($this->DS[$SRC]["DSC"])
		{
			$D = "d.htm?Image=".urlencode(base64_encode($SRC));
			$IMG .= " longdesc=\"d.htm?Image=".urlencode(base64_decode($SRC))."\"";
		}

		if (trim($Extra))
			$IMG .= " ".trim($Extra);
		$IMG .= " />";
		return($IMG);
	}

	function Put($SRC, $Extra = "")
	{
		echo $this->Get($SRC, $Extra);
	}

	function LoadImageData()
	{
		require_once($this->DataSource);
		$this->DS = $WAI_IMAGES;
	}

	function LoadImages()
	{
		$this->Images = array();

		foreach($this->Paths as $Path)
		{
			$D = dir($this->Relational.$Path);

			while($Entry = $D->read())
			{
				$Extension = strtolower(array_pop(explode(".", $Entry)));
				if (in_array($Extension, array("gif", "jpg", "png")))
					$this->Images[] = $Path.$Entry;
			}
			$D->close();
		}
		sort ($this->Images);
	}

	function SaveImages()
	{
		GLOBAL $WAI_SRC, $WAI_WID, $WAI_HEI, $WAI_ALT, $WAI_DSC;
		$SaveData = $GLOBALS["IData"];
		if (is_array($WAI_SRC))
		{
			$AD = "";
			$AD .= "<?php\n\n";
			$AD .= "\$WAI_IMAGES = array();\n\n";

			$c = sizeof($WAI_SRC);

			for ($i=0;$i<$c;$i++)
			{
				$AD .= "\$WAI_IMAGES[\"".$WAI_SRC[$i]."\"] = array(\n";
				$AD .= "\t\"WID\"=>\"".$WAI_WID[$i]."\"";
				$AD .= ",\n ";
				$AD .= "\t\"HEI\"=>\"".$WAI_HEI[$i]."\"";
				$AD .= ",\n ";
				$AD .= "\t\"ALT\"=>\"".base64_encode($WAI_ALT[$i])."\"";
				$AD .= ",\n ";
				$AD .= "\t\"DSC\"=>\"".base64_encode($WAI_DSC[$i])."\"";
				$AD .= "\n ";
				$AD .= ");\n";
			}
			$AD .= "?>";

			$fp = fopen ($this->DataSource, "w");
			fwrite($fp, $AD, strlen($AD));
			fclose($fp);
			$this->LoadImageData();
			return (1);
		}
		return (0);
	}

	function MaintainImages()
	{
		$this->LoadImages();
		echo "<STYLE>\nTD\n{\nfont-family: sans-serif;\nfont-size: 12px;\n}\n</STYLE>\n";

		Field::PutFormStart($_SERVER["REQUEST_URI"]);
		$T = new Table();
		$i = 0;
		foreach ($this->Images as $Image)
		{
			$Index = $this->Relational.$Image; 
			$IData = getimagesize($Index); 
			$T->AddData("<IMG SRC=\"".$Index."\" ".$IData[3]." BORDER=0>");
			$T->AddData($Image);
			$ImgData = $Image;
			$ImgData .= "<br />";
			$ImgData .= Field::GetHidden("WAI_SRC[".$i."]", $Image);
			$ImgData .= Field::GetHidden("WAI_WID[".$i."]", $IData[0]);
			$ImgData .= Field::GetHidden("WAI_HEI[".$i."]", $IData[1]);
			$ImgData .= Field::GetSmartText("WAI_ALT[".$i."]", base64_decode($this->DS[$Image]["ALT"]), 50);
			$ImgData .= "<br />";
			$ImgData .= Field::GetSmartArea("WAI_DSC[".$i."]", base64_decode($this->DS[$Image]["DSC"]), 35, 5, "WRAP=\"virtual\"");
			$T->AddData($ImgData);
			$T->EndRow();
			$i++;
		}
		$T->AddHead(Field::GetFormSubmit("Save WAI Image Data"), 2);
		$T->EndRow();
		$T->Put();
		Field::PutFormEnd();
	}
}

?>
