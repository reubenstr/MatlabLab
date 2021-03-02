% Black Box
%
% Working solution demo code
%
% Note: The solution code attempts to find the simplist step by step approach
% to opening the Black Box; the code may not be optimized nor use advanced
% level coding techniques

% clear all stored varibles and clear Command Window
clear, clc

% remove spaces between lines in the Command Window
format compact

disp ('Phase one: gather and process coded data')

disp ('Initialize the sensor system')

% close all serial ports; prevents a port 'in use' error
delete(instrfindall)
% create serial port interface with the indicated COM port number
socket = serial('COM17','BAUD',57600);
% open serial port
fopen(socket);
% read socket once to clear the empty buffer error
fscanf(socket,'%s');
% sensor board needs time to initialize
pause(2);

disp ('Collect raw brightness data')

% increments i from 1 to 840
for i = 1:840
    
    % read data in the serial buffer (formatted as a string)
    % convert the string into a number
    data = str2num(fscanf(socket,'%s'));
    
    raw_data(i) = data;
    
    % disply the incrementor and the data for verification of code correctness
    disp  ([i, raw_data(i)])
    
end

% plot the raw data for visual inspection required to normalize the data
plot(raw_data)


%%

% normalize raw luminous intensity values into a binary and clock
% representation. (zeros, ones, and twos)
% note: equation parameters found through observation.

disp('Normalize data into zeros, ones, twos')

for i = 1:length(raw_data)
    
    if raw_data(i) < 50
        normalized_data(i) = 0;
    end
    
    if (raw_data(i) > 50) && (raw_data(i) < 200)
        normalized_data(i) = 1;
    end
    
    if raw_data(i) > 200
        normalized_data(i) = 2;
    end
    
end

disp (normalized_data)

%%
% remove duplicate subsequent values

% because the sensor reads quicker than the serial code changes there will
% be duplicate data.

disp('Remove duplicate subsequent values')

valuebuffer = 255; % set value to any number other than 0, 1, or 2
i_cleaned_data = 0; % incrementor seperate from for loop incrementor

for i = 1:length(normalized_data)
    
    value = normalized_data(i);
    
    % valuebuffer holds previously stored data
    if value ~= valuebuffer % new value found
        
        i_cleaned_data = i_cleaned_data + 1;
        cleaned_data(i_cleaned_data) = value;
        valuebuffer = value;
        
    end
    
end

disp (cleaned_data);

%%

% remove clock from the data (numberic value of 2)

disp('Remove clock values from data')

% use index values to find then remove values of 2 instead of a loop
only_binary_data = cleaned_data(cleaned_data ~= 2);

disp (only_binary_data)

%%

% find the terminators and fill new matrix with values between terminators

disp('Extract data between terminators')

% the terminator is the character '#' which in binary is 00100011
terminator = [0,0,1,0,0,0,1,1]

terminator_1_index = 0;
terminator_2_index = 0;

for i = 1:length(only_binary_data) - 8
    
    count = 0;
    
    % begin scanning for matching values starting at the index value of i
    for scan_i = 1:8
        
        if only_binary_data(i + scan_i) == terminator(scan_i)
            
            count = count + 1;
            
        end
        
    end
        
    % if there was eight matches, then the terminator was found
    % store the index value where the terminator began
    if count == 8
        
        fprintf ('Terminator found beginning at index %1.0f \n', i)
        
        if (terminator_1_index == 0) terminator_1_index = i;
        elseif (terminator_2_index == 0) terminator_2_index = i;
        end
        
    end
    
end

% because the index value stored in terminator_1_index indicated the
% beginning of the terminator, skip index values of the first terminator
trimmed_data = only_binary_data(terminator_1_index + 9 : terminator_2_index);

disp (trimmed_data)

%%

disp ('Group binary values into bytes')

byte_data = vec2mat(trimmed_data, 8);

disp (byte_data)

%%

disp ('Convert bytes into decimal')

numberic_data = bin2dec(num2str(byte_data))';

disp (numberic_data)

%%

disp('Debug: convert decimal into ASCII')

% the results will show a well-known historical scientist's last name
% pending data collection was without error

disp (char(numberic_data))

%%

disp ('Phase two: apply decryption steps')

%%

disp ('Drop the maximum value(s)')

decrypted_data_1 = numberic_data(numberic_data ~= max(numberic_data))

%%

disp ('Drop the first even number')

for i = 1:length(decrypted_data_1)
    
    if rem(decrypted_data_1(i),2) == 0
        
        fprintf ('First even number found at index %1.0f \n', i)
        
        decrypted_data_2 = decrypted_data_1(decrypted_data_1 ~= decrypted_data_1(i))
        
        break;
        
    end
    
end

%%

disp ('Reverse array order')

decrypted_data_3 = fliplr(decrypted_data_2)

%%

disp ('Subtract from all values the rounded standard deviation of the values.')

decrypted_data_4 = decrypted_data_3 - round(std(decrypted_data_3))

%%

disp ('Phase three: Send Passcode')

%%

disp ('Convert decimal values in to binary')

% dec2bin() converts decimal values into a string with ASCII 0's and 1's
% convert ASCII 0's and 1's to decimal 0's and 1's for later processing

sending_binary_data = dec2bin(decrypted_data_4, 8) - 48;

disp (sending_binary_data)

%%

disp ('Convert matrix into an continuous array')

sending_continuous_data = vec2mat(sending_binary_data, 1)';

disp (sending_continuous_data)


%%

disp ('Add terminator')

terminator = [0,0,1,0,0,0,1,1];

sending_terminated_binary_data = [sending_continuous_data, terminator];

disp (sending_terminated_binary_data)

%%

disp ('Add a clock value of 2 between every other value')

sending_data = zeros(1,length(sending_terminated_binary_data) * 2) + 2;
sending_data(1:2:length(sending_terminated_binary_data) * 2) = sending_terminated_binary_data;

disp(sending_data)

%%

disp ('Send binary infomation over the LED')

% create infinite sending loop
% note: entering CTRL-C on the PC keyboard will break the loop

while (1)
    
    for i = 1:length(sending_data)
        
        stopasync(socket) % required before fprintf() for continuous data sending
        if (sending_data(i) == 0) fprintf(socket,'%c','0') % activate LED at low brightness
        end
        
        stopasync(socket) % required before fprintf() for continuous data sending
        if (sending_data(i) == 1) fprintf(socket,'%c','1') % activate LED at medium brightness
        end
        
        
        stopasync(socket) % required before fprintf() for continuous data sending
        if (sending_data(i) == 2) fprintf(socket,'%c','2') % activate LED at high brightness
        end
        
        disp (sending_data(i))
        
        pause(.025)
        
    end
    
end