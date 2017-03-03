<?php
require_once("definitions.inc");
require_once(ToIncludes("cw")."palette.inc");

echo "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" >\n";
echo "<head>\n";
echo "<title>[CWA] Colour Palette</title>\n";
echo "</head>\n";

palette::show();

echo "</body>\n";
echo "</html>\n";

?>
