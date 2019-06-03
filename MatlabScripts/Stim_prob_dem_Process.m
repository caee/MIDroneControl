% 3class_probability_Matrix_Process.m
% -------------------------------
% Author : Aleksander Lund & Carl Emil Elling 
% Date   : May 2019
%
% Process function for Online MI-BCI in OpenViBE. Based on democratic
% selection from the 3 classifiers
%
% Inputs: box_in: 3 streamed matrixes of probability values from 3 separate
% classifiers, classifying 3 classes. 
%
% Outputs: box_out: Normalized probability values
%          Serial output sending commands to custom Arduino scripts

function box_out = Stim_prob_dem_Process(box_in)

        if(~box_in.user_data.is_headerset)
            %Set output channels
            box_in.outputs{1}.header = box_in.inputs{1}.header;
            box_in.outputs{1}.header.nb_channels = 3;
			box_in.outputs{1}.header.channel_names = {'R','L','B'};
            box_in.user_data.is_headerset = 1;
            % Print the header in the console, so as to see what is being
            % sent
            disp('Input header is :')
            box_in.inputs{1}.header
            disp('Output header is :')
            box_in.outputs{1}.header
            
            %Setup of Arduino output. Some info will have to be entered
            %manually here due to constraints in MATLAB
            %CHANGE THIS:
            box_in.user_data.port = "COM6"; %Name of COM port where Arduino is connected.
            
            %Do not change this. Initializing Arduino output
            box_in.user_data.s = serial(box_in.user_data.port,'BaudRate',9600);
            fopen(box_in.user_data.s);
            box_in.user_data.ArduinoOutput = '<E047T050R050A050>';
            box_in.user_data.Result = 0;
        end
    % Test if input lengths are equal
    if (OV_getNbPendingInputChunk(box_in,1) == OV_getNbPendingInputChunk(box_in,2)) && (OV_getNbPendingInputChunk(box_in,1) == OV_getNbPendingInputChunk(box_in,3))
        % Iterating over the pending chunks on input 1 
        for i = 1:OV_getNbPendingInputChunk(box_in,1)
            % we pop the first chunk to be processed, note that box_in is used as the output variable to continue processing
            [box_in, start_time1, end_time1, Mat1] = OV_popInputBuffer(box_in,1);
            [box_in, start_time2, end_time2, Mat2] = OV_popInputBuffer(box_in,2);
            [box_in, start_time3, end_time3, Mat3] = OV_popInputBuffer(box_in,3);
            
            %Adding probabilities for each class together. 
            % These input probabilities assume:
            % first channel is right vs left classified
            % second channel is right vs forward classified
            % third channel is left vs forward classified
            A = Mat1(1) + Mat2(1); %Right
            B = Mat1(2) + Mat3(1); %Left
            C = Mat2(2) + Mat3(2); %Forward
            % 3 classifiers will have probabilities adding up to 300%.
            % Normalizing with the sum of all probabilities gives a max
            % probability of 66.67% for each class.
            O = [A B C]/sum([A,B,C]);
            if max(O)>0.5
                Vote1 = Mat1==max(Mat1);
                Vote2 = Mat2==max(Mat2);
                Vote3 = Mat3==max(Mat3);
                Counted = [Vote1(1)+Vote2(1),Vote1(2)+Vote3(1),Vote2(2)+Vote3(2)];
                box_in.user_data.Result = Counted==max(Counted);
            end
            
            %Steering
            if sum(box_in.user_data.Result==max(box_in.user_data.Result))==1
                if Result(1)==1 %threshold percentage is set empirically
                    tmpArduinoOutput = '<A065>'; %Go right
                elseif Result(2)==1
                    tmpArduinoOutput = '<A035>'; %Go left
                elseif Result(3)==1
                    tmpArduinoOutput = ' <E065> '; %Go forward
                else %If no threshold is exceeded
                    tmpArduinoOutput = '<E047T050R050A050>'; %stand still 
                end
            else
                tmpArduinoOutput = '<E047T050R050A050>'; %stand still 
            end
            %
            if ~strcmp(tmpArduinoOutput,box_in.user_data.ArduinoOutput)
%              fprintf("left: %f right: %f forward %f",O(1),O(2),O(3));
            box_in.user_data.ArduinoOutput = tmpArduinoOutput;
            fwrite(box_in.user_data.s,box_in.user_data.ArduinoOutput);
            end

            box_in.user_data.Result = 0;
            box_in = OV_addOutputBuffer(box_in,1,start_time1,end_time1,O);
            
        end
    end
	% we iterate over the pending chunks on input 1 (SIGNAL)
% 	for i = 1: OV_getNbPendingInputChunk(box_in,1)
% 		
% 		% we pop the first chunk to be processed, note that box_in is used as the output variable to continue processing
% 		[box_in, start_time, end_time, matrix_data] = OV_popInputBuffer(box_in,1);
%         
% 		box_in.outputs{1}.header = box_in.inputs{1}.header;
% 		
%         % if requested, all samples are set to 0
% 		if(box_in.user_data.trigger_state)
% 			matrix_data(1:end) = 0;
% 		end
% 		
% 		% we add the chunk to the signal output buffer. Note that we use box_in as the output variable.
% 		box_in = OV_addOutputBuffer(box_in,1,start_time,end_time,matrix_data);
% 		
%     end
    
    % Pass the modified box as output to continue processing
    box_out = box_in;
end