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

require_once(PHPLIB_DIR."variable.inc");
require_once(ToIncludes("audit")."audit.inc");
require_once(ToIncludes("audit")."file.inc");

class save_file extends variable
{
	function __construct()
	{
		parent::__construct();

		$allowedExtensions = array();
		// max file size in bytes
		$sizeLimit = 10 * 1024 * 1024;

		$uploader = new qqFileUploader($allowedExtensions, $sizeLimit);
		$result = $uploader->handleUpload('/tmp/');

		$this->MakeL("OperatorID", "AuditID");

		// Set params
		$Data = array();

		$Data["file_data"] = file_get_contents($uploader->SavedName);
		$Data["file_name"] = $uploader->Name;
		$Data["file_mime"] = $uploader->Type;
		$Data["file_size"] = filesize($uploader->SavedName);
		$Data["audit_id"] = $this->AuditID;
		$Data["operator_id"] = $this->OperatorID;

		$File = new file($this->OperatorID);

		// Save
		$File->SaveFile($Data);
		$File->SaveLink($this->AuditID);

		unlink($uploader->SavedName);

		//echo htmlspecialchars(json_encode($result, ENT_NOQUOTES));
		echo json_encode($result, ENT_NOQUOTES);
		exit;
	}

/*
	function __construct()
	{
		parent::__construct();

		if (!empty($_FILES))
		{
			$tempFile = $_FILES['Filedata']['tmp_name'];
			$targetPath = $_SERVER['DOCUMENT_ROOT'] . $_REQUEST['folder'] . '/';
			$targetFile =  str_replace('//','/',$targetPath) . $_FILES['Filedata']['name'];
   
			move_uploaded_file($tempFile,$targetFile);
			echo "1";

			//$this->MakeL("OperatorID", "AuditID");
			$this->MakeL("AuditID");

			// Set params
			$Data = array();

			$Data["file_data"] = get_file_contents($targetFile);
			$Data["file_name"] = $_FILES["Filedata"]["name"];
			$Data["file_mime"] = $_FILES["Filedata"]["type"];
			$Data["file_size"] = size_of($Data["file_data"]);
			$Data["audit_id"] = $this->AuditID;

			// Save
			$Audit = new audit($this->AuditID);
			$Audit->SaveFile($Data);
		}
		else

		exit();
	}
*/
}


/**
 * Handle file uploads via XMLHttpRequest
 */
class qqUploadedFileXhr {
    /**
     * Save the file to the specified path
     * @return boolean TRUE on success
     */
    function save($path) {    
        $input = fopen("php://input", "r");
        $temp = tmpfile();
        $realSize = stream_copy_to_stream($input, $temp);
        fclose($input);
        
        if ($realSize != $this->getSize()){            
            return false;
        }
        
        $target = fopen($path, "w");        
        fseek($temp, 0, SEEK_SET);
        stream_copy_to_stream($temp, $target);
        fclose($target);
        
        return true;
    }
    function getName() {
        return $_GET['qqfile'];
    }
    function getSize() {
        if (isset($_SERVER["CONTENT_LENGTH"])){
            return (int)$_SERVER["CONTENT_LENGTH"];            
        } else {
            throw new Exception('Getting content length is not supported.');
        }      
    }   

    function getType() {
	    if (isset($_SERVER["CONTENT_TYPE"])){
		    return $_SERVER["CONTENT_TYPE"];
	    }
    }
}

/**
 * Handle file uploads via regular form post (uses the $_FILES array)
 */
class qqUploadedFileForm {  
    /**
     * Save the file to the specified path
     * @return boolean TRUE on success
     */
    function save($path) {
        if(!move_uploaded_file($_FILES['qqfile']['tmp_name'], $path)){
            return false;
        }
        return true;
    }
    function getName() {
        return $_FILES['qqfile']['name'];
    }
    function getSize() {
        return $_FILES['qqfile']['size'];
    }
}


class qqFileUploader {
    private $allowedExtensions = array();
    //private $sizeLimit = 10485760;
    private $sizeLimit = 16777216;
    private $file;

    function __construct(array $allowedExtensions = array(), $sizeLimit = 16777216){        
        $allowedExtensions = array_map("strtolower", $allowedExtensions);
            
        $this->allowedExtensions = $allowedExtensions;        
        $this->sizeLimit = $sizeLimit;
        
        $this->checkServerSettings();       

        if (isset($_GET['qqfile'])) {
            $this->file = new qqUploadedFileXhr();
        } elseif (isset($_FILES['qqfile'])) {
            $this->file = new qqUploadedFileForm();
        } else {
            $this->file = false; 
        }
    }
    
    private function checkServerSettings(){        
        $postSize = $this->toBytes(ini_get('post_max_size'));
        $uploadSize = $this->toBytes(ini_get('upload_max_filesize'));        
        
        if ($postSize > $this->sizeLimit || $uploadSize > $this->sizeLimit){
            $size = max(1, $this->sizeLimit / 1024 / 1024) . 'M';             
       //     die("{'error':'increase post_max_size and upload_max_filesize to $size ($postSize | $uploadSize)'}");    
        }        
    }
    
    private function toBytes($str){
        $val = trim($str);
        $last = strtolower($str[strlen($str)-1]);
        switch($last) {
            case 'g': $val *= 1024;
            case 'm': $val *= 1024;
            case 'k': $val *= 1024;        
        }
        return $val;
    }
    
    /**
     * Returns array('success'=>true) or array('error'=>'error message')
     */
    function handleUpload($uploadDirectory, $replaceOldFile = FALSE){
        if (!is_writable($uploadDirectory)){
            return array('error' => "Server error. Upload directory isn't writable.");
        }
        
        if (!$this->file){
            return array('error' => 'No files were uploaded.');
        }
        
        $size = $this->file->getSize();
	    $this->Name = $this->file->getName();
	    $this->Type = $this->file->getType();

        
        if ($size == 0) {
            return array('error' => 'File is empty');
        }
        
        if ($size > $this->sizeLimit) {
            return array('error' => 'File is too large');
        }
        
        $pathinfo = pathinfo($this->file->getName());
        $filename = $pathinfo['filename'];
        //$filename = md5(uniqid());
        $ext = $pathinfo['extension'];

        if($this->allowedExtensions && !in_array(strtolower($ext), $this->allowedExtensions)){
            $these = implode(', ', $this->allowedExtensions);
            return array('error' => 'File has an invalid extension, it should be one of '. $these . '.');
        }
        
        if(!$replaceOldFile){
            /// don't overwrite previous files that were uploaded
            while (file_exists($uploadDirectory . $filename . '.' . $ext)) {
                $filename .= rand(10, 99);
            }
        }
      
	   	$this->SavedName = $uploadDirectory . $filename . '.' . $ext;
        if ($this->file->save($this->SavedName)){
            return array('success'=>true);
        } else {
            return array('error'=> 'Could not save uploaded file.' .
                'The upload was cancelled, or server error encountered');
        }
        
    } 
}

?>
