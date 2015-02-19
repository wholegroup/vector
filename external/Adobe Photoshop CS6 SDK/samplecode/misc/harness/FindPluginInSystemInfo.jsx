main();

function main() {
var errors = 0;
var sysInfo = app.systemInformation;
var failStart = sysInfo.search( "Plug-ins that failed to load:" );
if ( failStart == -1 )
    errors++;
var failEnd = sysInfo.search( "Flash:");
if ( failEnd == -1 )
    errors++;
if ( failEnd < failStart )
    errors++;
   
// TODO double check the statistics version, "Generic Plug-in" and 3dsimulator 
// one plug-in could have more than one entry, statistics and SelectoramaShape
// some plug-ins don't show up, see above TODO to confirm
var pluginList = [ "3DFormat", "Heightfield From Grayscale", "3DSimpleModeler", 
                   "3DSimpleRender", /* "3DSimulator", */ "AutomationFilter",
                   "ColorMunger", "Dissolve", "Getter", "GradientImport",
                   "Hidden", "History", "Listener", "Measurement Sample",
                   "MFC PlugIn", "NearestBase", "Outbound", 
                   "Poor Man's Type Tool", "Propetizer", "Selectorama", 
                   "Shape", "SimpleFormat", "Entropy", "Kurtosis", "Maximum",
                   "Mean", "Median", "Minimum", "Range", "Skewness", 
                   "Standard Deviation", "Summation", "Variance", "TextAuto",
                   "TextFormat" ];

var pluginIndex = [];

for ( var i = 0; i < pluginList.length; i++ ) {
    pluginIndex[i] = sysInfo.search( pluginList[i] );
    // $.writeln( pluginList[i] + " = " + pluginIndex[i] );
    if ( pluginIndex[i] == -1 )
        errors++;
    else if ( pluginIndex[i] > failStart && pluginIndex[i] < failEnd )
        errors++;
}

return errors == 0 ? " PASS" : " FAIL" + " " + errors;
}
// end FindPluginInSystemInfo.jsx
