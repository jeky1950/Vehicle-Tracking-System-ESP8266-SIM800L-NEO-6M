window.onload = function() { 
   toastr.success("Welcome, you are now connected to the Vehicle Tracking System.");
   dataUpdater();
   getThisDeviceData();
};

function dataUpdater(){
	returnServer();
	
	setInterval(function ( ) {
	   returnServer();
	}, 5000);
}

function updateAccessPointConfig(){
	//Update the station configuration
	let ap_ssid = $("#ap_ssid").val();
	let ap_password = $("#ap_password").val();
   
    if(ap_ssid && ap_password){
    	let number_of_characters = ap_password.length; //ap password characters must be atleast 8 characters long
    	if (number_of_characters >= 8) {
	    	bootbox.confirm("Are you sure?", function(result){
			   if(result){
			    	$("#change_ap_config").html("loading...");
					jQuery.post('/updateAccessPointConfig', {ap_ssid: ap_ssid, ap_password: ap_password}, function(data){
					  console.log(data);
					  toastr.success("WiFi Settings have been SUCCESSFULLY changed.", "", {timeOut: 10000});
					  toastr.info("Please Reconnect to the WiFi network of this Vehicle Tracking System and refresh this page again.", "", {timeOut: 10000});
					  $("#change_ap_config").html("Finished");
					});
				}
			});
	    }else{
	        toastr.warning("The password must be at least 8 characters long.");
	    }
    }else{
    	toastr.warning("Please enter the WiFi network name and password.");
    }
}

function updateWhitelistConfig(){
	//Update the station configuration
	let WtL_phone_number = $("#WtL_phone_number").val();
	WtL_phone_number = WtL_phone_number.replace("+","");
   
    if(WtL_phone_number){
    	bootbox.confirm("Are you sure?", function(result){
		   if(result){
		    	$("#change_ap_config").html("loading...");
				jQuery.post('/updateWtLConfig', {WtL_phone_number: WtL_phone_number}, function(data){
				  console.log(data);
				  toastr.success("Whitelist has been SUCCESSFULLY changed.", "", {timeOut: 10000});
				  $("#change_WtL").html("Finished");
				});
			}
		});
    }else{
    	toastr.warning("Please enter the Whitelist phone number.");
    }
}


function returnServer(){
	jQuery.getJSON('/returnData', function(json){
	  console.log(json);
 
      $("#networkOperator").html("<td>Network Operator</td><td>"+(json.operator_name ? json.operator_name : "--") + "</td>");

      $("#signalQuality").html("<td>Signal Quality</td><td>"+(json.signal_quality ? json.signal_quality : "--") + "</td>");

      $("#isRegistered").html("<td>Is Sim Registered to this network?</td><td>"+ (json.is_registered ? "TRUE" : "--") +"</td>");

      $("#numberOfSats").html("<td>Number of Satellites being tracked (Minimum: 4)</td><td>"+(json.number_of_satellites ? json.number_of_satellites : "--") + "</td>");

      $("#gpsLat").html("<td>GPS latitude</td><td>"+(json.vehicle_gps_lat ? json.vehicle_gps_lat + "°" : "--") + "</td>");

      $("#gpsLon").html("<td>GPS longitude</td><td>"+(json.vehicle_gps_lon ? json.vehicle_gps_lon + "°" : "--") + "</td>");

      $("#course").html("<td>Vehicle direction from Harare CBD</td><td>"+(json.vehicle_cardinal_direction ? json.vehicle_cardinal_direction + " ("+ (json.vehicle_course_degrees).toFixed(2) +"°)" : "--") + "</td>");

      $("#distanceFromHarare").html("<td>Vehicle distance from Harare CBD</td><td>"+(json.vehicle_distance_from_city ? (json.vehicle_distance_from_city).toFixed(2) + "km" : "--") + "</td>");

      $("#altitude").html("<td>Vehicle Altitude</td><td>"+(json.vehicle_altitude ? json.vehicle_altitude + "m" : "--") + "</td>");
      
      $("#speed").html("<td>Vehicle Speed</td><td>"+ (json.vehicle_speed + "kmph") + "</td>");
    })
	.fail(function() {
	   systemIsNowOffLine(); 
	});
}

function rebootDevice(){
	$("#reboot_btn").html("loading....");
	bootbox.confirm("Are you sure?", function(result){
		if(result){
			jQuery.post("/reboot", (data)=>{
				if(data == "success"){
					$("#reboot_btn").html("REBOOT DEVICE");
					console.log(data);
					toastr.info("The system is now restarting. Please reconnect to the WiFi network of the Vehicle Tracking Sevice to access the system again.", "", {timeOut: 8000});
					$(".station_status").html("<h5><font color='gray'>The system is now restarting. Please reconnect to the WiFi network of the Vehicle Tracking System to access the system again.</font></h5>");
			    }
			});
		}
	});
}

function getThisDeviceData(){
   jQuery.post('/returnAPConfig', function(data){
	  console.log(data);
	  data = JSON.parse(data);
	  $("#ap_ssid").val(data.ap_ssid);
   });

   jQuery.post('/returnWtLConfig', function(data){
	  console.log(data);
	  data = JSON.parse(data);
	  $("#WtL_phone_number").val(data.WtL_phone_number);
   });
}

function systemIsNowOffLine(){
	if($(".toast.toast-info").length == 0){
		isOffline = toastr.info("This system is now OFFLINE. Please ensure that you are connected to the WiFi network of the Vehicle Tracking System.", "", {timeOut: 0});
	}
}