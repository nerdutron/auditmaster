<?php

class file
{
	function __construct($OperatorID, $FileID = "")
	{
		$this->OperatorID = $OperatorID;
		$this->FileID = $FileID;
	}

	function SaveFile($FileData, $Description = "")
	{

		if (!$FileData["file_mime"])
		{
			$Extension = strtolower(array_pop(explode(".", $FileData["file_name"])));
			switch ($Extension)
			{
				case "doc":
				case "docx":
					$FileData["file_mime"] = "application/msword";
					break;
				case "pdf":
					$FileData["file_mime"] = "application/pdf";
					break;
			}
		}

//		fwrite($FP, "*****************************************\n".print_r($FileData, true));

		$INQ = new DBInsert();

		$INQ->Save("file_name", $FileData["file_name"]);
		$INQ->Save("file_data", $FileData["file_data"]);
		$INQ->Save("file_size", $FileData["file_size"]);
		$INQ->Save("description", $Description);
		$INQ->Save("operator_id", $this->OperatorID);
		$INQ->Add("FUNCTION", "loaded", "now()");

		$INQ->Save("file_mime", $FileData["file_mime"]);

		$INQ->Run(DB, "files", "INSERT");

		$this->FileID = $INQ->GetInsertID();

		$INQ->EndQuery();
	}

	function SaveLink($AuditID)
	{
		$INQ = new DBInsert();

		$INQ->Save("file_id", $this->FileID);
		$INQ->Save("audit_id", $AuditID);
		$INQ->Save("operator_id", $this->OperatorID);

		$INQ->Run(DB, "file_links", "INSERT");

		$INQ->EndQuery();
	}

	function DownloadFile($FileID)
	{
		$FileData = DBRow(DB, "SELECT * FROM files WHERE file_id = $FileID");

		header('Content-Description: File Transfer');
		header("Content-Disposition: attachment; filename=".urlencode($FileData["file_name"]));
        //header("Content-Length: ".$FileData["file_size"]);
        header("Content-Length: ".strlen($FileData["file_data"]));
        header("Content-Type: ".$FileData["file_mime"]);

        echo $FileData["file_data"];
	}

	function DeleteFile($FileID)
	{
		DB_DoQuery(DB, "DELETE FROM files WHERE file_id = $FileID");
		DB_DoQuery(DB, "DELETE FROM file_links WHERE file_id = $FileID");
	}
}
?>
