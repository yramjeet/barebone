function handleFile(f){
	const img = document.createElement('img');

	img.onload = function () {
		const canvas = document.createElement('canvas');
		// canvas.style = "width: 600";
		const ctx = canvas.getContext('2d');
		// console.log(img.width);
		canvas.width = 1404;
		canvas.height = 1872;
		let scale = Math.min(canvas.width / img.width, canvas.height / img.height);
		let width = img.width * scale;
		let height = img.height * scale;
		let dx = canvas.width / 2 - width / 2;
		let dy = canvas.height / 2 - height / 2;
		ctx.drawImage(img, dx, dy, width, height);
		const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
		const gs = new Uint8ClampedArray(1404*1872);
		let o = 0;
		const pixels = imageData.data;
		for (let i = 0; i < pixels.length; i += 4){
			gs[o] = (pixels[i] * 6966 + pixels[i+1] * 23436 + pixels[i+2] * 2366) >> 15;
			o++;
		}
		const w = canvas.width;
		for (let y = 0; y < canvas.height; y++) {
		    for (let x = 0; x < w; x++) {
				const i = y * w + x;

				// grayscale
				const oldPixel = gs[i];
				const newPixel = oldPixel < 128 ? 0 : 255;
				const error = oldPixel - newPixel;

				gs[i] = newPixel;

				// distribute error
				if (x + 1 < w) gs[i + i] += error * 7 / 16;
				if (y + 1 < canvas.height) {
					if (x > 0) gs[i + (w - 1)] += error * 3 / 16;
					gs[i + w] += error * 5 / 16;
					if (x + 1 < w) gs[i + (w + 1)] += error * 1 / 16;
				}
		    }
		}

		const fbgs = new Uint8ClampedArray(1404*1872/2);

		let z = 0;
		for (let i = 0; i < gs.length; i += 2){
			fbgs[z] = ((gs[i]/16) << 4) | (gs[i+1]/16) ;
			z++;
		}

		const flipped = new Uint8ClampedArray(1404*1872/2);

		for (let q = 0; q < 1872; q++){
			for (let p = 0; p < 1404/2; p ++){
				flipped[(702 * q) + 701 - p] = fbgs[(702 * q) +p];
			}
		}

		// const gsrgba = new Uint8ClampedArray(1404*1872*4);
		// let m = 0;
		// for (let i = 0; i < flipped.length; i++){
		// 	let gsc = ((flipped[i] & 0xF0) >> 4) * 16;
		// 	gsrgba[m] = gsc;
		// 	gsrgba[m + 1] = gsc;
		// 	gsrgba[m + 2] = gsc;
		// 	gsrgba[m + 3] = 255;

		// 	gsc = (flipped[i] & 0x0F) * 16;
		// 	gsrgba[m + 4] = gsc;
		// 	gsrgba[m + 5] = gsc;
		// 	gsrgba[m + 6] = gsc;
		// 	gsrgba[m + 7] = 255;

		// 	m += 8;
		// }

		// const nimgdt = new ImageData(gsrgba, 1404, 1872);
		// console.log(nimgdt);
		// console.log(imageData);


		// document.getElementById('root').appendChild(canvas);
		// canvas.width = nimgdt.width;
		// canvas.height = nimgdt.height;
		// ctx.putImageData(nimgdt, 0, 0);

		var blob = new Blob([flipped.buffer], {type: 'application/octet-stream'});
		var blobUrl = URL.createObjectURL(blob);

		const a = document.createElement('a')
		a.href = blobUrl;
		a.download = f.name.split('.')[0];
		document.body.appendChild(a)
		a.style.display = 'none'
		a.click()
		a.remove()

		setTimeout(() => window.URL.revokeObjectURL(blobUrl), 500)

	}

	img.src = URL.createObjectURL(f);
}

const input = document.getElementById("input");

function handleImages(){
	for(let i =0; i < input.files.length; i++){
		handleFile(input.files[i]);
	}
}

input.addEventListener("change", handleImages);