% threeclass_probability_Matrix_Uninitialize.m
% -------------------------------
% Author : Aleksander Lund & Carl Emil Elling 
% Date   : May 2019
%
% Uninitialize function for Online MI-BCI in OpenViBE. If needed,
% statistics can be extracted in this script. (eg. command history and the
% likes)
%
% Inputs: box_in: inputs from threclass_probability_Matrix_Process.m script
% This includes probability values
%
% Outputs: box_out: Normalized probability values
% 
function box_out = threeclass_probability_Matrix_Uninitialize(box_in)
    disp('Uninitializing the box...')
    
    %Closes serial connection to Arduino
	fclose(box_in.user_data.s);
    %disconnects and deletes all instrument objects
    instrreset;
    box_out = box_in;
end
    