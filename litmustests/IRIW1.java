class IRIW1 {
	int x = 0;
	int y = 0;
	int tmpx1 = 0;
	int tmpy1 = 0;
	int tmpx2 = 0;
	int tmpy2 = 0;

	class Thread1 extends Thread {
		public void run(){
			x = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmpx1 = x;
			tmpy1 = y;
		}
	}

	class Thread3 extends Thread {
		public void run(){
			y = 1;
		}
	}

	class Thread4 extends Thread {
		public void run(){
			tmpy2 = y;
			tmpx2 = x;
		}
	}

	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		Thread3 t3 = new Thread3();
		Thread4 t4 = new Thread4();
		t1.start();
		t2.start();
		t3.start();
		t4.start();

		try {
			t1.join();
			t2.join();
			t3.join();
			t4.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			IRIW1 iriw = new IRIW1();
			iriw.test();
			if(iriw.tmpy1 == 0 && iriw.tmpx1 == 1)
			{
				if(iriw.tmpy2 == 1 && iriw.tmpx2 == 0)
					break;
			}
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM IRIW Test PASSED after 100000 iterations");
		else
			System.out.println("ARM IRIW Test FAILED: problem occurs on iteration " + counter);
	}
}
