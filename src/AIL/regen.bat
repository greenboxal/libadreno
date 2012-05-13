echo "Regenerating the parser..."
flex -Pail_ ail.l 
bison -p ail_ -dy ail.y
pause
