% threeclass_probability_Matrix_Initialize.m
% -------------------------------
% Author : Aleksander Lund & Carl Emil Elling 
% Date   : May 2019
%
% Initialize function for Online MI-BCI in OpenViBE.
% Sets settings 
%
% Inputs: box_in: 3 streamed matrixes of probability values from 3 separate
% classifiers, classifying 3 classes. 
%
% Outputs: box_out: Normalized probability values
%          Serial output sending commands to custom Arduino scripts

function box_out = Stim_prob_Initialize(box_in)
	
        % we display the setting values
    disp('Box settings are:')
	for i=1:size(box_in.settings,2)
		fprintf('\t%s : %s\n',box_in.settings(i).name, num2str(box_in.settings(i).value)); 
    end
    
	% let's add a user-defined indicator to know if the output header is set
    box_in.user_data.is_headerset = false;
   
    fprintf('I was here!');
	
    box_out = box_in;
   
end
    
    