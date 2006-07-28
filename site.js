
function toggle_section( id )
{
  e = document.getElementById( id );
  if( e.style.display != 'none' )
    e.style.display = 'none';
  else
    e.style.display = 'block';
}