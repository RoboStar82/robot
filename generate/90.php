<?php

print 'uint16_t sin[91] = {';
for ($i = 0; $i <= 90; $i ++) {
    print ($i ? ', ' : ' ') . round(4096 * sin($i / 180 * M_PI));
}
print ' };' . "\n";

print 'uint16_t cos[91] = {';
for ($i = 0; $i <= 90; $i ++) {
    print ($i ? ', ' : ' ') . round(4096 * cos($i / 180 * M_PI));
}
print ' };' . "\n";
