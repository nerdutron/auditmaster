#!/usr/local/bin/php -C
<?php
echo "\n\n\n";
if (!is_file("definitions.inc"))
	exit("Error: this script must be called from a directory with a definitions.inc\n\n");

require_once("definitions.inc");

if (!defined("CMS_DB"))
	exit("Error: constant CMS_DB must be defined\n\n");

$Types = array("cms_text", "cms_text_formatted", "cms_html", "cms_html_advanced", "cms_news", "cms_faq", "cms_faq_summary");

$TypeShow = "\n\nValid Types:\n- ".implode("\n- ", $Types)."\n\n";

if ($argc != 3)
	exit("\n\nUsage: cms_create <cms_type> <cms_description>".$TypeShow);
$Parameters = $argv;
array_shift($Parameters);
$Type = array_shift($Parameters);
$Desc = array_shift($Parameters);

if (!in_array($Type, $Types))
	exit("Error: Invalid type".$TypeShow);
$Count = DBGet(CMS_DB, "SELECT count(*) FROM content_master WHERE description = '".addslashes($Desc)."'");
if ($Count)
	exit("\n\nError: Content description already exists".$TypeShow);

$INQ = new DBInsert;
$INQ->Save("data_type", $Type);
$INQ->Save("description", $Desc);
$INQ->Run(CMS_DB, "content_master", "INSERT");
$ID = $INQ->GetInsertID();
$INQ->EndQuery();

echo "cms::show(".$ID."); // ".$Desc;

echo "\n\n\n";

?>
