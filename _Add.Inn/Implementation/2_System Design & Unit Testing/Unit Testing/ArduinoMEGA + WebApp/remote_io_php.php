<!DOCTYPE html>
<html>
<head>
<title>Arduino - PHPoC Shield</title>
<meta name="viewport" content="width=device-width, initial-scale=0.7, maximum-scale=0.7">
<meta charset="utf-8">
<style>
body { text-align: center; font-size: width/2pt; }
h1 { font-weight: bold; font-size: width/2pt; }
h2 { font-weight: bold; font-size: width/2pt; }
button { font-weight: bold; font-size: width/2pt; }

</style>
<script>
var CMD_NONE				= 0x00;
var CMD_ARDUINO_SET_MODE	= 0x01;
var CMD_ARDUINO_SET_STATE	= 0x02;
var CMD_ARDUINO_GET_MODE	= 0x03;
var CMD_ARDUINO_GET_STATE	= 0x04;

var CMD_WEB_UPDATE_STATE	= 0x10;
var CMD_WEB_UPDATE_MODE		= 0x11;

var PIN_MODE_FLAG_IO	= 0x00;
var PIN_MODE_FLAG_COMM	= 0x10; // communication

var PIN_MODE_INPUT	= 0x00;
var PIN_MODE_OUTPUT	= 0x01;
var PIN_MODE_ANALOG	= 0x03;
var PIN_MODE_PWM	= 0x04;
var PIN_MODE_SPI	= 0x10;
var PIN_MODE_I2C	= 0x11;
var PIN_MODE_UART	= 0x12;

var PIN_STATE_OFF	= 0;
var PIN_STATE_ON	= 1;

var canvas_width = 450, canvas_height = 500;
var img_width = 405, img_height = 333;
var img_ratio = 0.7;
var arduino_img = new Image();
var pos_d0	= {x: 231, y: 5 };
var pos_d13 = {x: 130, y: 79 };
var pos_a0	= {x: 380, y: 107 };
var pos_a5	= {x: 343, y: 135 };

var pos_d_list = new Array();
var pos_a_list = new Array();

var canvas;
var ctx;
var ws = null;

var pin_d_mode = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
var pin_a_mode = [3, 3, 3, 3, 3, 3];

var pin_d_state = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
var pin_a_state = [0, 0, 0, 0, 0, 0];

var X_D_POS_LEVEL_0 = 25;
var X_D_POS_LEVEL_1 = 100;
var X_D_POS_LEVEL_2 = 190;
var X_D_POS_PWM_BTN = 280;
var X_D_POS_SLIDER	= 280;
var X_A_POS_LEVEL_0;
var X_A_POS_LEVEL_1;
var X_A_POS_LEVEL_2;
var X_A_POS_LEVEL_3;

var Y_D_START, Y_D_END;
var Y_A_START, Y_A_END;
var Y_GAP;

var BOX_HEIGHT		= 40;
var AN_BOX_WIDTH	= 70;
var DIO_BOX_WIDTH	= 70;
var ONOFF_BOX_WIDTH	= 80;
var STATE_BOX_WIDTH	= 70;
var PWM_WIDTH = 200;

var click_state = 0;
var mouse_xyra = {x:0, y:0, r:0.0, a:0.0};

arduino_img.src = "arduino_2.jpg";

var buffer = "";

function init()
{
	var width = window.innerWidth;
	var height = window.innerHeight;
	
	//if(width < height)
		ratio = (width - 50) / canvas_width;
	//else
		//ratio = (height - 50) / canvas_width;
	
	canvas_width = Math.round(canvas_width*ratio);
	canvas_height = Math.round(canvas_height*ratio);
	
	img_width = Math.round(img_width*ratio);
	img_height = Math.round(img_height*ratio);
	
	img_width *= img_ratio;
	img_height *= img_ratio;
	
	X_A_POS_LEVEL_0 = canvas_width - X_D_POS_LEVEL_0;
	X_A_POS_LEVEL_1 = canvas_width - X_D_POS_LEVEL_1;
	X_A_POS_LEVEL_2 = canvas_width - X_D_POS_LEVEL_2;
	X_A_POS_LEVEL_3 = canvas_width - 280;
	
	for (var pin_id = 0; pin_id <= 14; pin_id++) 
	{ 
		if(pin_id == 8)
			continue;
		
		var x = pos_d0.x + ( pos_d13.x -  pos_d0.x)/ 14.0 * pin_id;
		var y = pos_d0.y + ( pos_d13.y -  pos_d0.y)/ 14.0 * pin_id;;
		
		x = x * img_ratio * ratio + (canvas_width - img_width) / 2 - 100;
		y = y * img_ratio * ratio + (canvas_height - img_height);
		
		pos_d_list.push(new Array(Math.round(x), Math.round(y)));
	}
	
	for (var pin_id = 0; pin_id <= 5; pin_id++) 
	{ 
		var x = pos_a0.x + ( pos_a5.x -  pos_a0.x)/ 5.0 * pin_id;
		var y = pos_a0.y + ( pos_a5.y -  pos_a0.y)/ 5.0 * pin_id;;
		
		x = x * img_ratio * ratio + (canvas_width - img_width) / 2 - 100;
		y = y * img_ratio * ratio + (canvas_height - img_height);
		
		pos_a_list.push(new Array(Math.round(x), Math.round(y)));
	}

	Y_D_START	= 30;
	Y_D_END		= pos_d_list[13][1] - 20;
	Y_GAP		= Math.round((Y_D_END - Y_D_START) / 14);
	Y_A_START	= Y_D_START +  5 * Y_GAP;
	Y_A_END		= Y_D_START + 10 * Y_GAP;;
	
	canvas = document.getElementById("canvas");
	ctx = canvas.getContext("2d");

	canvas.width = canvas_width;
	canvas.height = canvas_height;

	canvas.addEventListener("touchstart", mouse_down);
	canvas.addEventListener("touchend", mouse_up);
	canvas.addEventListener("touchmove", mouse_move);
	canvas.addEventListener("mousedown", mouse_down);
	canvas.addEventListener("mouseup", mouse_up);
	canvas.addEventListener("mousemove", mouse_move);
	
	update_view();
}
function connect_onclick()
{
	if(ws == null)
	{
		var ws_host_addr = "<?echo _SERVER("HTTP_HOST")?>";
		if((navigator.platform.indexOf("Win") != -1) && (ws_host_addr.charAt(0) == "["))
		{
			// network resource identifier to UNC path name conversion
			ws_host_addr = ws_host_addr.replace(/[\[\]]/g, '');
			ws_host_addr = ws_host_addr.replace(/:/g, "-");
			ws_host_addr += ".ipv6-literal.net";
		}
		
		ws = new WebSocket("ws://" + ws_host_addr + "/arduino_io", "text.phpoc");
		document.getElementById("ws_state").innerHTML = "CONNECTING";
		ws.onopen = ws_onopen;
		ws.onclose = ws_onclose;
		ws.onmessage = ws_onmessage;
	}
	else
		ws.close();
}
function ws_onopen()
{
	document.getElementById("ws_state").innerHTML = "<font color='blue'>CONNECTED</font>";
	document.getElementById("bt_connect").innerHTML = "Disconnect";
	
	setTimeout(function(){
		for(var pin = 0; pin <= 13; pin ++)
		{
			send_to_Arduino("D" + pin, CMD_ARDUINO_GET_MODE, 0);
			send_to_Arduino("D" + pin, CMD_ARDUINO_GET_STATE, 0);
		}
		
		for(var pin = 0; pin <= 5; pin ++)
		{
			send_to_Arduino("A" + pin, CMD_ARDUINO_GET_MODE, 0);
			send_to_Arduino("A" + pin, CMD_ARDUINO_GET_STATE, 0);
		}
	}, 100);
}
function ws_onclose()
{
	document.getElementById("ws_state").innerHTML = "<font color='gray'>CLOSED</font>";
	document.getElementById("bt_connect").innerHTML = "Connect";
	ws.onopen = null;
	ws.onclose = null;
	ws.onmessage = null;
	ws = null;
	update_view();
}
function ws_onmessage(e_msg)
{
	e_msg = e_msg || window.event; // MessageEvent
	
	buffer += e_msg.data;
	buffer = buffer.replace(/\r\n/g, "\n");
	buffer = buffer.replace(/\r/g, "\n");
	
	while(buffer.indexOf("\n") == 0)
		buffer = buffer.substr(1);
	
	while(buffer.indexOf("\n") >= 0) // because may update data come at the same time 
	{
		var str = buffer.substr(0, buffer.indexOf("\n"));
		buffer = buffer.substr(buffer.indexOf("\n") + 1);
		
		var arr		= str.split(":");
		var pin		= arr[0];
		var cmd		= parseInt(arr[1]);
		var state	= arr[2];
		
		var pin_group = pin.charAt(0);
		var pin_number = pin.substr(1);
		
		if(cmd == CMD_WEB_UPDATE_MODE)
		{
			if(pin_group == "D")
				pin_d_mode[pin_number]	= parseInt(state);
			else
			if(pin_group == "A")
				pin_a_mode[pin_number]	= parseInt(state);
		}
		else
		if(cmd == CMD_WEB_UPDATE_STATE)
		{
			if(pin_group == "D")
			{
				if(pin_d_mode[pin_number] == PIN_MODE_INPUT || pin_d_mode[pin_number] == PIN_MODE_OUTPUT)
					state = parseInt(state);

				pin_d_state[pin_number]	= state;
			}
			else
			if(pin_group == "A")
			{
				if(pin_a_mode[pin_number] == PIN_MODE_INPUT || pin_a_mode[pin_number] == PIN_MODE_OUTPUT)
					state = parseInt(state);

				pin_a_state[pin_number]	= state;
			}
		}
	}
	
	update_view();
}
function draw_pin(x, y, pin)
{
	ctx.fillStyle = "#00979d";
	ctx.beginPath();
	ctx.arc(x, y, 15, 0, 2 * Math.PI);
	ctx.fill();
	
	ctx.fillStyle = "white";
	ctx.fillText(pin, x, y);
}
function draw_analog_mode(x, y, mode)
{
	ctx.fillStyle = "Gray";
	ctx.beginPath();
	ctx.arc(x - (AN_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0.5 * Math.PI, 1.5 * Math.PI);
	ctx.arc(x + (AN_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 1.5 * Math.PI, 0.5 * Math.PI);
	ctx.fill();
	
	ctx.fillStyle = "DeepPink";
	ctx.beginPath();
	if(mode == PIN_MODE_ANALOG)
		ctx.arc(x + (AN_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0, 2 * Math.PI);
	else
		ctx.arc(x - (AN_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0, 2* Math.PI);
	
	ctx.fill();
	
	ctx.fillStyle = "white";
	ctx.fillText("A", x + (AN_BOX_WIDTH - BOX_HEIGHT) / 2, y);
	ctx.fillText("D", x - (AN_BOX_WIDTH - BOX_HEIGHT) / 2, y);
}
function draw_digital_mode(x, y, mode)
{
	ctx.fillStyle = "Gray";
	ctx.beginPath();
	ctx.arc(x - (DIO_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0.5 * Math.PI, 1.5 * Math.PI);
	ctx.arc(x + (DIO_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 1.5 * Math.PI, 0.5 * Math.PI);
	ctx.fill();
	
	ctx.fillStyle = "red";
	ctx.beginPath();
	
	if(mode == PIN_MODE_OUTPUT)
		ctx.arc(x + (DIO_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0, 2* Math.PI);
	else
		ctx.arc(x - (DIO_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0, 2 * Math.PI);
	
	ctx.fill();
	
	ctx.fillStyle = "white";
	ctx.fillText("DI", x - (DIO_BOX_WIDTH - BOX_HEIGHT) / 2, y);
	ctx.fillText("DO", x + (DIO_BOX_WIDTH - BOX_HEIGHT) / 2, y);
}
function draw_on_off(x, y, state)
{
	ctx.fillStyle = "Gray";
	ctx.beginPath();
	ctx.arc(x - (ONOFF_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0.5 * Math.PI, 1.5 * Math.PI);
	ctx.arc(x + (ONOFF_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 1.5 * Math.PI, 0.5 * Math.PI);
	ctx.fill();

	ctx.fillStyle = "#1E90FF";
	ctx.beginPath();
	
	if(state == PIN_STATE_ON)
	{
		ctx.arc(x + (ONOFF_BOX_WIDTH - BOX_HEIGHT) / 2 - 5, y, BOX_HEIGHT / 2, 0.5 * Math.PI, 1.5 * Math.PI);
		ctx.arc(x + (ONOFF_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 1.5 * Math.PI, 0.5 * Math.PI);
	}
	else
	{
		ctx.arc(x - (ONOFF_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0.5 * Math.PI, 1.5 * Math.PI);
		ctx.arc(x - (ONOFF_BOX_WIDTH - BOX_HEIGHT) / 2 + 5, y, BOX_HEIGHT / 2, 1.5 * Math.PI, 0.5 * Math.PI);
	}
		
	ctx.fill();
	
	ctx.fillStyle = "white";
	ctx.fillText("OFF", x - (ONOFF_BOX_WIDTH - BOX_HEIGHT) / 2, y);
	ctx.fillText("ON", x + (ONOFF_BOX_WIDTH - BOX_HEIGHT) / 2, y);
}
function draw_comm(x, y, state)
{
	ctx.fillStyle = "Gray";
	ctx.beginPath();
	ctx.arc(x - (STATE_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0.5 * Math.PI, 1.5 * Math.PI);
	ctx.arc(x + (STATE_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 1.5 * Math.PI, 0.5 * Math.PI);
	ctx.closePath();
	ctx.fill();
	
	ctx.fillStyle = "White";
	ctx.fillText(state, x, y);
}
function draw_state(x, y, state)
{
	ctx.save();
	ctx.strokeStyle="#1E90FF";
	ctx.fillStyle = "white";
	ctx.lineWidth = 6;
	ctx.beginPath();
	ctx.arc(x - (STATE_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 0.5 * Math.PI, 1.5 * Math.PI);
	ctx.arc(x + (STATE_BOX_WIDTH - BOX_HEIGHT) / 2, y, BOX_HEIGHT / 2, 1.5 * Math.PI, 0.5 * Math.PI);
	ctx.closePath();
	ctx.fill();
	ctx.stroke();
	
	ctx.fillStyle = "#1E90FF";
	ctx.fillText(state, x, y);
	ctx.restore();
}
function draw_pwm_button(x, y, state)
{
	if(state == "PWM")
		ctx.fillStyle = "#FFA500";
	else
		ctx.fillStyle = "red";
	
	ctx.beginPath();
	ctx.arc(x, y, BOX_HEIGHT / 2, 0, 2 * Math.PI);
	ctx.closePath();
	ctx.fill();
	
	ctx.fillStyle = "White";
	ctx.fillText(state, x, y);
}
function draw_slider(x, y, value)
{
	ctx.save();
	ctx.strokeStyle="#1E90FF";
	ctx.lineWidth = 12;
	ctx.beginPath();
	ctx.lineTo(x - PWM_WIDTH / 2, y + 0.5);
	ctx.lineTo(x + PWM_WIDTH / 2, y + 0.5);
	ctx.stroke();
	
	x = x - PWM_WIDTH / 2 + value * PWM_WIDTH / 255;
	ctx.fillStyle = "#FF4500";
	ctx.beginPath();
	ctx.arc(x , y, BOX_HEIGHT / 2, 0, 2 * Math.PI);
	ctx.fill();
	ctx.restore();
}
function update_view()
{
	ctx.clearRect(0, 0, canvas_width, canvas_height);
	ctx.drawImage(arduino_img, (canvas_width - img_width) / 2 - 100, canvas_height - img_height, img_width, img_height);

	ctx.lineWidth = 3;
	ctx.strokeStyle="gray";
	ctx.globalAlpha=1;
	ctx.font="bold 16px Arial";
	ctx.textBaseline="middle"; 
	ctx.textAlign="center";

	for (var pin_id = 0; pin_id <= 13; pin_id++) 
	{
		ctx.fillStyle = "DeepPink";
		ctx.beginPath();
		ctx.arc(pos_d_list[pin_id][0] + 0.5, pos_d_list[pin_id][1] + 0.5, 4, 0, 2 * Math.PI);
		ctx.fill();
		
		var x, y; 
		
		x = 0;
		
		if(pin_id < 8)
			y = Y_D_START + pin_id * Y_GAP;
		else
			y = Y_D_START + (pin_id + 1) * Y_GAP;
		
		ctx.beginPath();
		ctx.lineTo(x  + 0.5, y + 0.5);
		ctx.lineTo(pos_d_list[pin_id][0] + 0.5, y + 0.5);
		ctx.lineTo(pos_d_list[pin_id][0] + 0.5, pos_d_list[pin_id][1] + 0.5);
		ctx.stroke();
		
		draw_pin(X_D_POS_LEVEL_0, y, "D" + pin_id.toString());
		
		if(pin_d_mode[pin_id] & PIN_MODE_FLAG_COMM)
		{
			if(pin_d_mode[pin_id] == PIN_MODE_UART)
				draw_comm(X_D_POS_LEVEL_1, y, "UART");
			else
			if(pin_d_mode[pin_id] == PIN_MODE_SPI)
				draw_comm(X_D_POS_LEVEL_1, y, "SPI");
			else
			if(pin_d_mode[pin_id] == PIN_MODE_I2C)
				draw_comm(X_D_POS_LEVEL_1, y, "I2C");
			
			draw_state(X_D_POS_LEVEL_2, y, pin_d_state[pin_id]);
			continue;
		}
		
		if(pin_d_mode[pin_id] == PIN_MODE_PWM)
		{
			draw_pwm_button(X_D_POS_LEVEL_1, y, "I/O")
			draw_slider(X_D_POS_PWM_BTN, y, pin_d_state[pin_id]);
		}
		else
		{
			draw_digital_mode(X_D_POS_LEVEL_1, y, pin_d_mode[pin_id]);
			
			if(pin_d_mode[pin_id] == PIN_MODE_OUTPUT)
				draw_on_off(X_D_POS_LEVEL_2, y, pin_d_state[pin_id])
			else
			{
				if(pin_d_state[pin_id] == PIN_STATE_ON)
					draw_state(X_D_POS_LEVEL_2, y, "ON");
				else
					draw_state(X_D_POS_LEVEL_2, y, "OFF");
			}
			
			if(pin_id == 3 || pin_id == 5 || pin_id == 6 || pin_id == 9 )
				draw_pwm_button(X_D_POS_PWM_BTN, y, "PWM")
		}
	}
	
	for (var pin_id = 0; pin_id <= 5; pin_id++) 
	{
		ctx.fillStyle = "DeepPink";
		ctx.beginPath();
		ctx.arc(pos_a_list[pin_id][0] + 0.5, pos_a_list[pin_id][1] + 0.5, 4, 0, 2 * Math.PI);
		ctx.fill();
		
		var x, y; 
		
		x = canvas_width;
		y = Y_A_END - pin_id * Y_GAP;
		
		ctx.beginPath();
		ctx.lineTo(x  + 0.5, y + 0.5);
		ctx.lineTo(pos_a_list[pin_id][0] + 0.5, y + 0.5);
		ctx.lineTo(pos_a_list[pin_id][0] + 0.5, pos_a_list[pin_id][1] + 0.5);
		ctx.stroke();
		
		draw_pin(X_A_POS_LEVEL_0, y, "A" + pin_id.toString());
		draw_analog_mode(X_A_POS_LEVEL_1, y, pin_a_mode[pin_id]);
		
		if(pin_a_mode[pin_id] == PIN_MODE_ANALOG)
			draw_state(X_A_POS_LEVEL_2, y, pin_a_state[pin_id]);
		else
		{
			draw_digital_mode(X_A_POS_LEVEL_2, y, pin_a_mode[pin_id]);
			
			if(pin_a_mode[pin_id] == PIN_MODE_OUTPUT)
				draw_on_off(X_A_POS_LEVEL_3, y, pin_a_state[pin_id])
			else
			{
				if(pin_a_state[pin_id] == PIN_STATE_ON)
					draw_state(X_A_POS_LEVEL_3, y, "ON");
				else
					draw_state(X_A_POS_LEVEL_3, y, "OFF");
			}
		}
	}
}
function process_event(event)
{
	if(event.offsetX)
	{
		touch_x = event.offsetX;
		touch_y = event.offsetY;
	}
	else if(event.layerX)
	{
		touch_x = event.layerX;
		touch_y = event.layerY;
	}
	else
	{
		touch_x = Math.round(event.touches[0].pageX - event.touches[0].target.offsetLeft);
		touch_y = Math.round(event.touches[0].pageY - event.touches[0].target.offsetTop);
	}
	
	var pin;
	var cmd = CMD_NONE;
	var state = 0;
		
	var offset = (touch_y - Y_D_START) % Y_GAP;
	var pin_id  = Math.round((touch_y - Y_D_START - offset) / Y_GAP);
	
	if((offset < BOX_HEIGHT / 2) || (offset > (Y_GAP - BOX_HEIGHT / 2) && offset < Y_GAP))
	{
		if(offset > (Y_GAP - BOX_HEIGHT / 2) && offset < Y_GAP)
				pin_id++;
			
		if(touch_x < (canvas_width / 2))
		{
			if(pin_id > 7)
				pin_id--;
			
			if(pin_id < 0 || pin_id > 13)
				return;
			
			if(pin_d_mode[pin_id] & PIN_MODE_FLAG_COMM)
				return;
			
			if(pin_d_mode[pin_id] != PIN_MODE_PWM)
			{
				if(event_type == "move")
					return;
				
				if( touch_x > (X_D_POS_LEVEL_1 - DIO_BOX_WIDTH / 2) && touch_x < (X_D_POS_LEVEL_1 + DIO_BOX_WIDTH / 2))
				{
					pin_d_mode[pin_id] = (pin_d_mode[pin_id] + 1) % 2;
					cmd = CMD_ARDUINO_SET_MODE;
					state = pin_d_mode[pin_id];
				}
				else
				if( touch_x > (X_D_POS_LEVEL_2 - ONOFF_BOX_WIDTH / 2) && touch_x < (X_D_POS_LEVEL_2 + ONOFF_BOX_WIDTH / 2))
				{
					if(pin_d_mode[pin_id] == PIN_MODE_OUTPUT)
					{
						pin_d_state[pin_id] = (pin_d_state[pin_id] + 1) % 2;
						cmd = CMD_ARDUINO_SET_STATE;
						state = pin_d_state[pin_id];
					}
				}
				else
				if( touch_x > (X_D_POS_PWM_BTN - BOX_HEIGHT / 2) && touch_x < (X_D_POS_PWM_BTN + BOX_HEIGHT / 2))
				{
					pin_d_mode[pin_id] = PIN_MODE_PWM;
					pin_d_state[pin_id] = 0;
					cmd = CMD_ARDUINO_SET_MODE;
					state = pin_d_mode[pin_id];
				}
			}
			else
			{
				if( touch_x > (X_D_POS_LEVEL_1 - BOX_HEIGHT / 2) && touch_x < (X_D_POS_LEVEL_1 + BOX_HEIGHT / 2))
				{
					if(event_type == "move")
						return;

					pin_d_mode[pin_id] = PIN_MODE_OUTPUT;
					pin_d_state[pin_id] = 0;
					cmd = CMD_ARDUINO_SET_MODE;
					state = pin_d_mode[pin_id];
				}
				else
				{
					var x_slider = X_D_POS_SLIDER - PWM_WIDTH / 2 + pin_d_state[pin_id] * PWM_WIDTH / 255;
					if( touch_x > (x_slider - PWM_WIDTH / 2) && touch_x < (x_slider + PWM_WIDTH / 2))
					{
						state = (touch_x - (X_D_POS_SLIDER - PWM_WIDTH / 2 )) / PWM_WIDTH * 255;
						
						if(state >= 0 && state <= 255)
						{
							pin_d_state[pin_id] = Math.round(state);
							cmd = CMD_ARDUINO_SET_STATE;
						}
					}
				}
			}
			
			pin = "D" + pin_id;
		}
		else
		{
			if(event_type == "move")
				return;
			
			pin_id -= 5;
			
			if(pin_id < 0 || pin_id > 5)
				return;
			
			pin_id = 5 - pin_id;
			
			if( touch_x > (X_A_POS_LEVEL_1 - AN_BOX_WIDTH / 2) && touch_x < (X_A_POS_LEVEL_1 + AN_BOX_WIDTH / 2))
			{
				if(pin_a_mode[pin_id] == PIN_MODE_ANALOG)
				{
					pin_a_mode[pin_id] = PIN_MODE_INPUT;
					pin_a_state[pin_id] = PIN_STATE_OFF;
				}
				else
				{
					pin_a_mode[pin_id] = PIN_MODE_ANALOG;
					pin_a_state[pin_id] = 0;
				}
				
				cmd = CMD_ARDUINO_SET_MODE;
				state = pin_a_mode[pin_id];
			}
			else
			if( touch_x > (X_A_POS_LEVEL_2 - DIO_BOX_WIDTH / 2) && touch_x < (X_A_POS_LEVEL_2 + DIO_BOX_WIDTH / 2))
			{
				if(pin_a_mode[pin_id] != PIN_MODE_ANALOG)
				{
					pin_a_mode[pin_id] = (pin_a_mode[pin_id] + 1) % 2;
					cmd = CMD_ARDUINO_SET_MODE;
					state = pin_a_mode[pin_id];
				}
			}
			else
			if( touch_x > (X_A_POS_LEVEL_3 - ONOFF_BOX_WIDTH / 2) && touch_x < (X_A_POS_LEVEL_3 + ONOFF_BOX_WIDTH / 2))
			{
				if(pin_a_mode[pin_id] == PIN_MODE_OUTPUT)
				{
					pin_a_state[pin_id] = (pin_a_state[pin_id] + 1) % 2;
					cmd = CMD_ARDUINO_SET_STATE;
					state = pin_a_state[pin_id];
				}
			}
			
			pin = "A" + pin_id;
		}	
	}
	
	if(cmd != CMD_NONE)
	{
		update_view();
		send_to_Arduino(pin, cmd, state);
	}
}
function mouse_down()
{
	//if(ws == null || authorized)
		//return;
	
	event.preventDefault();
	event_type = "down";
	process_event(event);
}
function mouse_up()
{
	event.preventDefault();
	
	//if(ws != null && authorized == false)
		//send_to_Arduino(CMD_AUTH, touch_list.toString());
	
	event_type = "up";
	update_view();
}
function mouse_move()
{
	event.preventDefault();
	
	event_type = "move";
	process_event(event);
}

function send_to_Arduino(pin, cmd, state)
{
	if(ws.readyState == 1)
	{
		ws.send(pin + ":" + cmd + ":" + state + "\r\n");
		console.log("send data:" + pin + ":" + cmd + ":" + state);
	}
}

window.onload = init;
</script>
</head>

<body>

<p>
</p>
<canvas id="canvas"></canvas>
<h2>
<p>WebSocket : <span id="ws_state">null</span></p>
<button id="bt_connect" type="button" onclick="connect_onclick();">Connect</button>
</h2>

</body>
</html>