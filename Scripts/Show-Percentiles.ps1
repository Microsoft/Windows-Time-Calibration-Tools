﻿# From http://www.dummies.com/education/math/statistics/how-to-calculate-percentiles-in-statistics/

Param(
   [Parameter(Mandatory=$True,Position=1)]
   [string]$File
)

$obj = type $file | ConvertFrom-Csv -header time, a1, a2, a3, a4 | sort @{expression={$_.a1 -as [double]} }

$percentiles = 0.68, 0.95, 0.997 

$percentiles  | foreach {
    $pp = $obj.count * $_

    if($_ -eq [math]::Round($x_)){
        $p = [math]::Round($pp + 1)
        echo ("The " + $_ + " percentile = " + $obj[$p - 1].a1)
    } else {
        echo ("The " + $_ + " percentile = " + $obj[$pp - 1].a1)
    }
}
#sort @{expression={$_.date -as [datetime]}} 