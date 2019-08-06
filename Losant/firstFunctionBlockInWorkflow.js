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

payload.data.body.fingerprint_id = parseInt(payload.data.body.data.substring(0,2));
payload.data.body.admin_choice = parseInt(payload.data.body.data.substring(2,4));
payload.data.body.admin_id_modified = parseInt(payload.data.body.data.substring(4,6));
if(payload.data.body.admin_choice == 1){
  payload.data.body.alert = "Added a new fingerprint. ID = "+payload.data.body.admin_id_modified;
}
else if(payload.data.body.admin_choice == 2){
  payload.data.body.alert = "Erased a fingerprint. ID = "+payload.data.body.admin_id_modified;
}
else if(payload.data.body.admin_choice == 0){
  payload.data.body.alert = "Entry";
}
