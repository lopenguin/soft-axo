figure; hAxes = gca;
plot( 1:25, rand(1,25) )
uicontrol('Style', 'slider', 'Min',1,'Max',50,'Value',41, 'Position', [400 20 120 20], 'Callback', @(src,evt) hax( src, hAxes ) ); 
function hax( src, hAxes )
    xlim( [0 get( src, 'Value' )] )
end