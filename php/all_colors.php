<?php
    include("colors_class.php");
    // Create new Colors class
    $colors = new Colors();

    // Get Foreground Colors
    $fgs = $colors->getForegroundColors();
    // Get Background Colors
    $bgs = $colors->getBackgroundColors();

    // Loop through all foreground and background colors
    $count = count($fgs);
    for ($i = 0; $i < $count; $i++) {
        echo $colors->getColoredString("Test Foreground colors", $fgs[$i]) . "\t";
        if (isset($bgs[$i])) {
            echo $colors->getColoredString("Test Background colors", null, $bgs[$i]);
        }
        echo "\n";
    }
    echo "\n";

    // Loop through all foreground and background colors
    foreach ($fgs as $fg) {
        foreach ($bgs as $bg) {
            echo $colors->getColoredString("Test Colors", $fg, $bg) . "\t";
        }
        echo "\n";
    }
?>