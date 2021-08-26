
N=21140; 
Bino_n = 50;
Bino_p = 0.62;
TotalWeight=zeros(N,1); 
for k=1:N;
	U = rand(5*Bino_n, 1); 
	i = sum(U < Bino_p);
	Y = i;  
	weight = 0; 
	
	s = 0; t = 8; m = 0.22;  
	for f=1:Y; 
		CX = 0; CY = m; F = 0; 
		while (CY > F); 
			U = rand; V = rand;
			CX = s+(t-s)*U; CY = m*V;
			
			if CX <= 2
				F = 0.07*CX;
			elseif CX <=5
				F = -0.02*(CX-4)^2 + 0.22;
                        elseif CX <=7
				F = 0.08*(5-CX) + 0.2;
			else
				F = -0.04*CX + 0.32;
			end
		end;
		weight = weight + CX;
	end;
	TotalWeight(k) = weight;
end;
p_est = mean(TotalWeight>640);
expectedWeight = mean(TotalWeight);
stdWeight = std(TotalWeight);
fprintf('Estimated probability = %f\n',p_est);
fprintf('Expected weight = %f\n',expectedWeight);
fprintf('Standard deviation = %f\n',stdWeight);
