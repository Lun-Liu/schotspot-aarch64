class WRW_2W1Array {
	int[] x = {0,0};
	int tmp = 0;

	class Thread1 extends Thread {
		public void run(){
			x[0] = 2;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmp = x[0];
			x[1] = 1;
		}
	}

	class Thread3 extends Thread {
		public void run(){
			x[1] = 2;
			x[0] = 1;
		}
	}


	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		Thread3 t3 = new Thread3();

		t1.start();
		t2.start();
		t3.start();

		try {
			t1.join();
			t2.join();
			t3.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			WRW_2W1Array wrw = new WRW_2W1Array();
			wrw.test();
			if(wrw.tmp == 2 && wrw.x[0] == 2 && wrw.x[1] == 2) 
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM WRW+2W Test PASSED after 100000 iterations");
		else
			System.out.println("ARM WRW+2W Test FAILED: problem occurs on iteration " + counter);
	}
}
