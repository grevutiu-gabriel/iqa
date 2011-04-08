function [l, c, s] = ssim_wang(img1, img2, a, b, g)
    C1 = (0.01*255)^2;
    C2 = (0.03*255)^2;
    C3 = C2/2;
    window = fspecial('gaussian', 11, 1.5);
    window = window/sum(sum(window));

    %Reduce to single precision float.
    window = round(window * 1000000.0)/1000000.0;

    mu1   = filter2(window, img1, 'valid');
    mu2   = filter2(window, img2, 'valid');
    mu1_sq = mu1.*mu1;
    mu2_sq = mu2.*mu2;
    mu1_mu2 = mu1.*mu2;
    sigma1_sq = filter2(window, img1.*img1, 'valid') - mu1_sq;
    sigma2_sq = filter2(window, img2.*img2, 'valid') - mu2_sq;
    sigma12 = filter2(window, img1.*img2, 'valid') - mu1_mu2;
    
    sigma1 = sigma1_sq.^0.5;
    sigma2 = sigma2_sq.^0.5;
    lum = (2*mu1_mu2 + C1) ./ (mu1_sq + mu2_sq + C1);
    con = (2*sigma1.*sigma2 + C2) ./ (sigma1_sq + sigma2_sq + C2);
    stc = (sigma12 + C3) ./ (sigma1.*sigma2 + C3);
    [M,N] = size(lum);
    aa = repmat(a,M,N);
    bb = repmat(b,M,N);
    gg = repmat(g,M,N);
    ssim_map = real(lum.^aa) .* real(con.^bb) .* real(stc.^gg);
end

function ssim = ssim_rouse(img1, img2, weight)
    ssim = ones(2,2).*weight;
end


%==============================================================================
% Usage: ms_ssim(img1, img2, 0)
%==============================================================================
function msssim = ms_ssim(img1, img2, wang)

if (nargin < 2 || nargin > 4)
    msssim = -Inf;
    return;
end

if (size(img1) ~= size(img2))
    msssim = -Inf;
    return;
end

if (nargin < 3)
    wang = 0;
end
if (nargin < 5)
    scale = 5;
    alphas = [0.0    0.0    0.0    0.0    0.1333];
    betas  = [0.0448 0.2856 0.3001 0.2363 0.1333];
    gammas = [0.0448 0.2856 0.3001 0.2363 0.1333];
end

[M N] = size(img1);
img1 = double(img1);
img2 = double(img2);

% Filter for downsampling.
lod = [0.0378 -0.0238 -0.1106 0.3774 0.8527 0.3774 -0.1106 -0.0238 0.0378]';
lpf = lod * lod';
lpf = lpf/sum(sum(lpf));

% Run SSIM on each scale.
for n = 1:scale
    if (wang == 1)
        ssim{n} = ssim_wang(img1,img2,alphas(n),betas(n),gammas(n));
    else
        ssim{n} = ssim_rouse(img1,img2,alphas(n),betas(n),gammas(n));
    end
    
    msssim_map = msssim_map.*ssim(n);
    
    % Downsample
    img1 = imfilter(img1,lpf,'symmetric','same');
    img2 = imfilter(img2,lpf,'symmetric','same');
    img1 = img1(1:2:end,1:2:end);
    img2 = img2(1:2:end,1:2:end);
end

msssim = mean2(msssim_map);
return;