% threeclass_probability_Matrix_Initialize.m
% -------------------------------
% Author : Aleksander Lund & Carl Emil Elling 
% Date   : May 2019
%
% Initialize function for Online MI-BCI in OpenViBE.
% Sets box settings.
%
% Inputs: box_in: 3 streamed matrixes of probability values from 3 separate
% classifiers, classifying 3 classes. 
%
% Outputs: box_out: information sent to process script, including settings
% and user data

function box_out = threeclass_probability_Matrix_Initialize(box_in)
	
        % we display the setting values
    disp('Box settings are:')
	for i=1:size(box_in.settings,2)
		fprintf('\t%s : %s\n',box_in.settings(i).name, num2str(box_in.settings(i).value)); 
    end
    
	% adding user data to (in process function) determine if output header
	% and arduino output is set
    box_in.user_data.is_headerset = false;
    
    box_out = box_in;
   
end
    
    