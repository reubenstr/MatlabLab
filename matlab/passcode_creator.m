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

numberic_data = ('Pascal')

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







