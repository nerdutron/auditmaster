<?php

$Stuff = array(array("id" => 10, "name" => "Bob"));

foreach ($Stuff as $Row)
{
	$Row["extra"] = "extra";
}

print_r($Stuff);

?>
