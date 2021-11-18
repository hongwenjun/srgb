<?php
    include("colors_class.php");
    // Create new Colors class
    $colors = new Colors();

    // Test some basic printing with Colors class
    echo $colors->getColoredString("Testing Colors class, this is purple string on yellow background.", "purple", "yellow") . "\n";
    echo $colors->getColoredString("Testing Colors class, this is blue string on light gray background.", "blue", "light_gray") . "\n";
    echo $colors->getColoredString("Testing Colors class, this is red string on black background.", "red", "black") . "\n";
    echo $colors->getColoredString("Testing Colors class, this is cyan string on green background.", "cyan", "green") . "\n";
    echo $colors->getColoredString("Testing Colors class, this is cyan string on default background.", "cyan") . "\n";
    echo $colors->getColoredString("Testing Colors class, this is default string on cyan background.", null, "cyan") . "\n";
?>