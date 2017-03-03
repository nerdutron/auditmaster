<?php
// mailq.php - mail q sender processor
// runs: every minute

require_once("definitions.inc");
require_once(PHPLIB_DIR."mailq.inc");

$Args = $argv;
array_shift($Args);
MailQ::Delete($Args);
echo "\n\nItems Deleted\n";

?>
