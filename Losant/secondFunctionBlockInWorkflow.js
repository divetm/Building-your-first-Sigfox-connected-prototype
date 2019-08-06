/*
 * payload variable contains current payload.
 * e.g. payload.data.myValue
 *
 * You can modify the current payload variable
 * or return an object that will entirely
 * replace the payload.
 *
 * console.log() will write a message to
 * the Debug tab.
 *
 * The Buffer object is available for
 * complex parsing: https://nodejs.org/dist/latest-v6.x/docs/api/buffer.html
*/

payload.data.body.nombre = ascii_to_hexa(payload.data.body.name.items[0].Nombre);

function ascii_to_hexa(str)
  {
	var arr1 = [];
	for (var n = 0, l = str.length; n < l; n ++) 
     {
		var hex = Number(str.charCodeAt(n)).toString(16);
		arr1.push(hex);
	 }
	return arr1.join('');
   }

if (payload.data.body.nombre.length < 16){
    var n = 16-payload.data.body.nombre.length
    for (var i=0; i<n;i++){
      payload.data.body.nombre += "0";
	 }
}
else if (payload.data.body.nombre.length > 16){
    payload.data.body.nombre = payload.data.body.nombre.substr(0,16);
}
